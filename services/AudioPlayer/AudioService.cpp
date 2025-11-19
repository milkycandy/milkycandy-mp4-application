#include "AudioService.h"
#include "framework/core/Dispatcher.h"
#include <chrono>
#include <thread>

AudioService& AudioService::getInstance() {
    static AudioService instance;
    return instance;
}

AudioService::AudioService() {
    player_ = std::make_unique<GStreamerAudioPlayer>();
    // Set default callbacks for EOS and error to just log or no-op; UI will handle as needed
    player_->setOnEos([this]() {
        state_.store(PlaybackState::Stopped);
        notifyStateChange();
    });
    player_->setOnError([](const std::string& reason) {
        // For now, no UI toast; can be hooked by activities if needed
        (void)reason;
    });
    player_->setOnMetadata([this](const std::string& title, const std::string& artist){
        notifyMetadata(title, artist);
    });
}

AudioService::~AudioService() {
    stopProgressSampling();
    stop();
}

void AudioService::init() {
    // 预热：确保 player 存在即可；进度采样按需启动（有监听时启动）
    if (!player_) {
        player_ = std::make_unique<GStreamerAudioPlayer>();
    }
}

bool AudioService::setSource(const std::string& path) {
    if (!player_) return false;
    // 如果与当前源相同，则不重建管线，保持现状
    if (path == currentSource_) {
        return true;
    }
    bool ok = player_->setSource(path);
    if (ok) {
        player_->setVolume(defaultVolume_);
        state_.store(PlaybackState::Paused);
        notifyStateChange();
        currentSource_ = path;
    }
    return ok;
}

bool AudioService::play() {
    if (!player_) return false;
    bool ok = player_->play();
    if (ok) {
        state_.store(PlaybackState::Playing);
        notifyStateChange();
    }
    return ok;
}

bool AudioService::pause() {
    if (!player_) return false;
    bool ok = player_->pause();
    if (ok) {
        state_.store(PlaybackState::Paused);
        notifyStateChange();
    }
    return ok;
}

bool AudioService::stop() {
    if (!player_) return false;
    bool ok = player_->stop();
    if (ok) {
        state_.store(PlaybackState::Stopped);
        notifyStateChange();
    }
    return ok;
}

bool AudioService::seekMs(int64_t posMs) {
    if (!player_) return false;
    return player_->seekMs(posMs);
}

bool AudioService::setVolume(float v) {
    if (!player_) return false;
    return player_->setVolume(v);
}

int64_t AudioService::getDurationMs() const {
    if (!player_) return -1;
    return player_->getDurationMs();
}

int64_t AudioService::getPositionMs() const {
    if (!player_) return -1;
    return player_->getPositionMs();
}

PlaybackState AudioService::getState() const {
    return state_.load();
}

int AudioService::addProgressListener(ProgressListener cb) {
    bool shouldStart = false;
    int id;

    {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        id = progressNextId_++;
        listeners_[id] = std::move(cb);
        
        // 检查是否需要启动（读取 atomic 是线程安全的）
        if (!samplingRunning_.load()) {
            shouldStart = true;
        }
    } // 锁在这里释放

    // 在锁外启动线程，避免 AB-BA 死锁
    if (shouldStart) {
        startProgressSampling();
    }

    return id;
}

void AudioService::removeProgressListener(int id) {
    bool shouldStop = false;
    {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        listeners_.erase(id);
        // 检查是否为空，但不立即停止，只设置标志位
        if (listeners_.empty()) {
            shouldStop = true;
        }
    } // 锁在这里释放

    // 在锁外执行停止和 join 操作，避免死锁
    if (shouldStop) {
        stopProgressSampling();
    }
}

int AudioService::addMetadataListener(MetadataListener cb) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    int id = metadataNextId_++;
    metadataListeners_[id] = std::move(cb);
    return id;
}

void AudioService::removeMetadataListener(int id) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    metadataListeners_.erase(id);
}

int AudioService::addStateListener(StateListener cb) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    int id = stateNextId_++;
    stateListeners_[id] = std::move(cb);
    return id;
}

void AudioService::removeStateListener(int id) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    stateListeners_.erase(id);
}

void AudioService::startProgressSampling() {
    std::lock_guard<std::mutex> lock(threadLifecycleMutex_);
    if (samplingRunning_.load()) return;
    samplingRunning_.store(true);
    samplingThread_ = std::thread([this]() { samplingLoop(); });
}

void AudioService::stopProgressSampling() {
    std::lock_guard<std::mutex> lock(threadLifecycleMutex_);
    if (!samplingRunning_.load()) return;
    samplingRunning_.store(false);
    if (samplingThread_.joinable()) samplingThread_.join();
}

void AudioService::samplingLoop() {
    using namespace std::chrono_literals;
    while (samplingRunning_.load()) {
        int64_t pos = getPositionMs();
        int64_t dur = getDurationMs();
        
        std::vector<ProgressListener> listenersCopy;
        {
            std::lock_guard<std::mutex> lock(listenersMutex_);
            if (!listeners_.empty()) {
                for (auto& kv : listeners_) {
                    if (kv.second) listenersCopy.push_back(kv.second);
                }
            }
        }
        
        if (!listenersCopy.empty()) {
            runOnUiThread([listenersCopy, pos, dur]() {
                for (const auto& cb : listenersCopy) {
                    if (cb) cb(pos, dur);
                }
            });
        }
        
        std::this_thread::sleep_for(200ms);
    }
}

void AudioService::notifyMetadata(const std::string& title, const std::string& artist) {
    std::vector<MetadataListener> listenersCopy;
    
    {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        if (metadataListeners_.empty()) return;
        for (auto& kv : metadataListeners_) {
            if (kv.second) listenersCopy.push_back(kv.second);
        }
    }
    
    runOnUiThread([listenersCopy, title, artist]() {
        for (const auto& cb : listenersCopy) {
            if (cb) cb(title, artist);
        }
    });
}

void AudioService::notifyStateChange() {
    std::vector<StateListener> listenersCopy;
    PlaybackState currentState;

    {
        std::lock_guard<std::mutex> lock(listenersMutex_);
        if (stateListeners_.empty()) return;
        currentState = state_.load();
        for (auto& kv : stateListeners_) {
            if (kv.second) listenersCopy.push_back(kv.second);
        }
    }

    runOnUiThread([listenersCopy, currentState]() {
        for (const auto& cb : listenersCopy) {
            if (cb) cb(currentState);
        }
    });
}