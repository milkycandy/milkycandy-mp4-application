#pragma once
#include <functional>
#include <string>
#include <cstdint>

class AudioPlayer {
public:
    virtual ~AudioPlayer() = default;

    virtual bool setSource(const std::string& path) = 0;
    virtual bool play() = 0;
    virtual bool pause() = 0;
    virtual bool stop() = 0;

    virtual bool setVolume(float volume_0_to_1) = 0;
    virtual bool seekMs(int64_t position_ms) = 0;

    virtual int64_t getDurationMs() const = 0;
    virtual int64_t getPositionMs() const = 0;

    // 回调：可选
    void setOnEos(const std::function<void()>& cb) { onEos_ = cb; }
    void setOnError(const std::function<void(const std::string&)>& cb) { onError_ = cb; }

protected:
    std::function<void()> onEos_;
    std::function<void(const std::string&)> onError_;
};