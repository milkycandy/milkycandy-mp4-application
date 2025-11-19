#include "GStreamerAudioPlayer.h"
#include <algorithm>
#include <cstring>
#include "framework/core/Dispatcher.h"

namespace {
std::once_flag gstOnceFlag;
}

void GStreamerAudioPlayer::ensureGstInitialized() {
    std::call_once(gstOnceFlag, []() {
        gst_init(nullptr, nullptr);
    });
}

GStreamerAudioPlayer::GStreamerAudioPlayer() {
    ensureGstInitialized();
    loop_ = g_main_loop_new(nullptr, FALSE);
    loopRunning_.store(true);
    loopThread_ = std::thread([this]() {
        g_main_loop_run(loop_);
        loopRunning_.store(false);
    });
}

GStreamerAudioPlayer::~GStreamerAudioPlayer() {
    stop();
    teardownPipeline();
    if (loop_) {
        g_main_loop_quit(loop_);
        if (loopThread_.joinable()) loopThread_.join();
        g_main_loop_unref(loop_);
        loop_ = nullptr;
    }
}

std::string GStreamerAudioPlayer::getLowercaseExt(const std::string& path) {
    auto pos = path.find_last_of('.');
    if (pos == std::string::npos) return "";
    std::string ext = path.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool GStreamerAudioPlayer::setSource(const std::string& path) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    teardownPipeline();
    return buildPipelineFor(path);
}

bool GStreamerAudioPlayer::buildPipelineFor(const std::string& path) {
    const std::string ext = getLowercaseExt(path);

    pipeline_ = gst_pipeline_new("audio_pipeline");
    if (!pipeline_) return false;

    // 先创建局部元素，确保在加入 pipeline 之前失败可安全释放
    GstElement* queue    = gst_element_factory_make("queue", "q");
    GstElement* convert  = gst_element_factory_make("audioconvert", "convert");
    GstElement* resample = gst_element_factory_make("audioresample", "resample");
    GstElement* volume   = gst_element_factory_make("volume", "volume");
    GstElement* sink     = gst_element_factory_make("pipewiresink", "sink");
    if (!queue || !convert || !resample || !volume || !sink) {
        // 释放已创建的局部元素与 pipeline
        if (queue)    gst_object_unref(queue);
        if (convert)  gst_object_unref(convert);
        if (resample) gst_object_unref(resample);
        if (volume)   gst_object_unref(volume);
        if (sink)     gst_object_unref(sink);
        gst_object_unref(pipeline_);
        pipeline_ = nullptr;
        return false;
    }

    // 安全设置属性：仅在属性存在时设置，避免 GLib-GObject-CRITICAL
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(sink), "client-name")) {
        g_object_set(sink, "client-name", "MyApp Audio", NULL);
    }
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(sink), "node-properties")) {
        GstStructure* node_props = gst_structure_from_string("node.name=Music Output", NULL);
        if (node_props) {
            g_object_set(sink, "node-properties", node_props, NULL);
            gst_structure_free(node_props);
        }
    }
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(sink), "stream-properties")) {
        GstStructure* stream_props = gst_structure_from_string("media.role=Music", NULL);
        if (stream_props) {
            g_object_set(sink, "stream-properties", stream_props, NULL);
            gst_structure_free(stream_props);
        }
    }

    GstElement* filesrc = gst_element_factory_make("filesrc", "src");
    if (!filesrc) {
        // 释放未加入 pipeline 的局部元素与 pipeline
        gst_object_unref(queue);
        gst_object_unref(convert);
        gst_object_unref(resample);
        gst_object_unref(volume);
        gst_object_unref(sink);
        gst_object_unref(pipeline_);
        pipeline_ = nullptr;
        return false;
    }
    g_object_set(filesrc, "location", path.c_str(), NULL);

    bool ok = false;

    if (ext == ".mp3") {
        GstElement* parser  = gst_element_factory_make("mpegaudioparse", "parser");
        GstElement* decoder = gst_element_factory_make("mpg123audiodec", "decoder");
        if (!parser || !decoder) {
            // 释放未加入 pipeline 的局部元素与 pipeline
            if (parser)  gst_object_unref(parser);
            if (decoder) gst_object_unref(decoder);
            gst_object_unref(filesrc);
            gst_object_unref(queue);
            gst_object_unref(convert);
            gst_object_unref(resample);
            gst_object_unref(volume);
            gst_object_unref(sink);
            gst_object_unref(pipeline_);
            pipeline_ = nullptr;
            return false;
        }

        // 将元素加入 pipeline
        gst_bin_add_many(GST_BIN(pipeline_), filesrc, parser, decoder, queue, convert, resample, volume, sink, NULL);
        // 在成功加入后立刻更新成员指针，避免 decodebin 分支中的 pad-added 回调竞态问题（此分支不使用 decodebin，但保持一致性）
        queue_ = queue;
        convert_ = convert;
        resample_ = resample;
        volume_ = volume;
        sink_ = sink;
        ok = gst_element_link_many(filesrc, parser, decoder, queue, convert, resample, volume, sink, NULL);
    } else {
        GstElement* decodebin = gst_element_factory_make("decodebin", "decodebin");
        if (!decodebin) {
            gst_object_unref(filesrc);
            gst_object_unref(queue);
            gst_object_unref(convert);
            gst_object_unref(resample);
            gst_object_unref(volume);
            gst_object_unref(sink);
            gst_object_unref(pipeline_);
            pipeline_ = nullptr;
            return false;
        }

        gst_bin_add_many(GST_BIN(pipeline_), filesrc, decodebin, queue, convert, resample, volume, sink, NULL);
        // 加入后先更新成员指针，确保 pad-added 回调能够访问到 queue_
        queue_ = queue;
        convert_ = convert;
        resample_ = resample;
        volume_ = volume;
        sink_ = sink;
        if (!gst_element_link(filesrc, decodebin)) {
            teardownPipeline();
            return false;
        }

        g_signal_connect(decodebin, "pad-added", G_CALLBACK(GStreamerAudioPlayer::onDecodebinPadAdded), this);

        ok = gst_element_link_many(queue, convert, resample, volume, sink, NULL);
    }

    if (!ok) {
        teardownPipeline();
        return false;
    }

    GstBus* bus = gst_element_get_bus(pipeline_);
    // 记录 watch id，在 teardown 时移除
    bus_watch_id_ = gst_bus_add_watch(bus, &GStreamerAudioPlayer::onBusMessage, this);
    gst_object_unref(bus);

    gst_element_set_state(pipeline_, GST_STATE_PAUSED);
    return true;
}

void GStreamerAudioPlayer::teardownPipeline() {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    // 先移除 Bus watch，避免销毁后仍回调
    if (bus_watch_id_) {
        g_source_remove(bus_watch_id_);
        bus_watch_id_ = 0;
    }

    if (pipeline_) {
        gst_element_set_state(pipeline_, GST_STATE_NULL);
        gst_object_unref(pipeline_);
        pipeline_ = nullptr;
    }
    queue_ = convert_ = resample_ = volume_ = sink_ = nullptr;
}

void GStreamerAudioPlayer::onDecodebinPadAdded(GstElement* decodebin, GstPad* new_pad, gpointer user_data) {
    auto* self = static_cast<GStreamerAudioPlayer*>(user_data);
    if (!self) return;

    std::unique_lock<std::recursive_mutex> lock(self->mtx_);

    if (!self->pipeline_ || !self->queue_) {
        return; 
    }

    // 仅处理 SRC pad
    if (gst_pad_get_direction(new_pad) != GST_PAD_SRC) {
        return;
    }

    GstPad* sinkpad = gst_element_get_static_pad(self->queue_, "sink");
    if (!sinkpad) return;

    if (gst_pad_is_linked(sinkpad)) {
        gst_object_unref(sinkpad);
        return;
    }

    GstCaps* caps = gst_pad_get_current_caps(new_pad);
    bool isAudio = false;
    if (caps) {
        const GstStructure* st = gst_caps_get_structure(caps, 0);
        if (st) {
            const gchar* name = gst_structure_get_name(st);
            isAudio = name && g_str_has_prefix(name, "audio/");
        }
        gst_caps_unref(caps);
    }

    if (isAudio) {
        (void)gst_pad_link(new_pad, sinkpad);
    }
    gst_object_unref(sinkpad);
}

gboolean GStreamerAudioPlayer::onBusMessage(GstBus*, GstMessage* msg, gpointer user_data) {
    auto* self = static_cast<GStreamerAudioPlayer*>(user_data);
    if (!self) return TRUE;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_TAG: {
            GstTagList* tags = nullptr;
            gst_message_parse_tag(msg, &tags);
            if (tags) {
                gchar* title = nullptr;
                gchar* artist = nullptr;
                gst_tag_list_get_string(tags, GST_TAG_TITLE, &title);
                gst_tag_list_get_string(tags, GST_TAG_ARTIST, &artist);
                std::string t = title ? title : "";
                std::string a = artist ? artist : "";
                if (title) g_free(title);
                if (artist) g_free(artist);
                if (!t.empty() || !a.empty()) {
                    self->emitMetadata(t, a);
                }
                gst_tag_list_unref(tags);
            }
            break;
        }
        case GST_MESSAGE_ERROR: {
            GError* err = nullptr;
            gchar* dbg = nullptr;
            gst_message_parse_error(msg, &err, &dbg);
            std::string reason = err ? err->message : "unknown error";
            if (self->onError_) {
                runOnUiThread([cb = self->onError_, reason] {
                    cb(reason);
                });
            }
            if (err) g_error_free(err);
            if (dbg) g_free(dbg);
            break;
        }
        case GST_MESSAGE_EOS: {
            if (self->onEos_) {
                runOnUiThread([cb = self->onEos_] {
                    cb();
                });
            }
            break;
        }
        default:
            break;
    }
    return TRUE;
}

bool GStreamerAudioPlayer::play() {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!pipeline_) return false;
    return gst_element_set_state(pipeline_, GST_STATE_PLAYING) != GST_STATE_CHANGE_FAILURE;
}

bool GStreamerAudioPlayer::pause() {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!pipeline_) return false;
    return gst_element_set_state(pipeline_, GST_STATE_PAUSED) != GST_STATE_CHANGE_FAILURE;
}

bool GStreamerAudioPlayer::stop() {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!pipeline_) return false;
    return gst_element_set_state(pipeline_, GST_STATE_NULL) != GST_STATE_CHANGE_FAILURE;
}

bool GStreamerAudioPlayer::setVolume(float v) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!volume_) return false;
    if (v < 0.f) v = 0.f;
    if (v > 1.f) v = 1.f;
    g_object_set(volume_, "volume", v, NULL);
    return true;
}

bool GStreamerAudioPlayer::seekMs(int64_t position_ms) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!pipeline_) return false;
    return gst_element_seek_simple(
        pipeline_, GST_FORMAT_TIME,
        (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
        position_ms * GST_MSECOND
    );
}

int64_t GStreamerAudioPlayer::getDurationMs() const {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!pipeline_) return -1;
    gint64 dur = 0;
    if (gst_element_query_duration(pipeline_, GST_FORMAT_TIME, &dur)) {
        return dur / GST_MSECOND;
    }
    return -1;
}

int64_t GStreamerAudioPlayer::getPositionMs() const {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (!pipeline_) return -1;
    gint64 pos = 0;
    if (gst_element_query_position(pipeline_, GST_FORMAT_TIME, &pos)) {
        return pos / GST_MSECOND;
    }
    return -1;
}
void GStreamerAudioPlayer::emitMetadata(const std::string& title, const std::string& artist) {
    if (onMetadata_) {
        runOnUiThread([cb = onMetadata_, title, artist](){
            cb(title, artist);
        });
    }
}