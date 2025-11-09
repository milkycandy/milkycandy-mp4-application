#pragma once
#include "AudioPlayer.h"
#include <gst/gst.h>
#include <atomic>
#include <mutex>
#include <thread>

class GStreamerAudioPlayer : public AudioPlayer {
public:
    GStreamerAudioPlayer();
    ~GStreamerAudioPlayer() override;

    bool setSource(const std::string& path) override;
    bool play() override;
    bool pause() override;
    bool stop() override;

    bool setVolume(float volume_0_to_1) override;
    bool seekMs(int64_t position_ms) override;

    int64_t getDurationMs() const override;
    int64_t getPositionMs() const override;

private:
    static void ensureGstInitialized();

    bool buildPipelineFor(const std::string& path);
    void teardownPipeline();

    static void onDecodebinPadAdded(GstElement* decodebin, GstPad* new_pad, gpointer user_data);
    static gboolean onBusMessage(GstBus* bus, GstMessage* msg, gpointer user_data);

    static std::string getLowercaseExt(const std::string& path);

private:
    GstElement* pipeline_ = nullptr;
    GstElement* volume_ = nullptr;
    GstElement* convert_ = nullptr;
    GstElement* resample_ = nullptr;
    GstElement* sink_ = nullptr;

    GMainLoop* loop_ = nullptr;
    std::thread loopThread_;
    std::atomic<bool> loopRunning_{false};
    mutable std::mutex mtx_;

    guint bus_watch_id_ = 0;
};