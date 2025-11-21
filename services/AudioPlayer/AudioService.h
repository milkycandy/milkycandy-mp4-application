#pragma once
#include <functional>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <string>
#include <thread>
#include "GStreamerAudioPlayer.h"

enum class PlaybackState { Stopped, Paused, Playing };

class AudioService {
public:
    static AudioService& getInstance();

    // Core controls
    bool setSource(const std::string& path);
    bool play();
    bool pause();
    bool stop();
    bool seekMs(int64_t posMs);
    bool setVolume(float v);

    // Query
    int64_t getDurationMs() const;
    int64_t getPositionMs() const;
    PlaybackState getState() const;
    const std::string& getCurrentSource() const { return currentSource_; }

    // Progress listeners (UI thread callbacks)
    using ProgressListener = std::function<void(int64_t position_ms, int64_t duration_ms)>;
    int addProgressListener(ProgressListener cb);
    void removeProgressListener(int id);

    // Metadata listeners: title and artist
    using MetadataListener = std::function<void(const std::string& title, const std::string& artist)>;
    int addMetadataListener(MetadataListener cb);
    void removeMetadataListener(int id);

    // Playback state listeners
    using StateListener = std::function<void(PlaybackState state)>;
    int addStateListener(StateListener cb);
    void removeStateListener(int id);

    // Init and teardown (optional)
    void init();

private:
    AudioService();
    ~AudioService();

    void startProgressSampling();
    void stopProgressSampling();
    void samplingLoop();
    void notifyMetadata(const std::string& title, const std::string& artist);
    void notifyStateChange();

private:
    std::unique_ptr<GStreamerAudioPlayer> player_;
    std::atomic<PlaybackState> state_{PlaybackState::Stopped};
    std::recursive_mutex listenersMutex_;
    std::unordered_map<int, ProgressListener> listeners_;
    int progressNextId_ = 1;

    std::unordered_map<int, MetadataListener> metadataListeners_;
    std::unordered_map<int, StateListener> stateListeners_;
    int metadataNextId_ = 1;
    int stateNextId_ = 1;

    // Progress sampling thread
    std::thread samplingThread_;
    std::atomic<bool> samplingRunning_{false};
    std::mutex threadLifecycleMutex_;  // 保护线程的创建和销毁
    // Default volume (hard-coded for now)
    float defaultVolume_ = 0.80f;

    // Track current source path to avoid reloading same track
    std::string currentSource_;
};