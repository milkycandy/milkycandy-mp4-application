#include "MusicActivity.h"
#include "framework/activity/ActivityManager.h"
#include "framework/ui/widgets/Dialog.h"
#include "framework/ui/widgets/Toast.h"
#include "framework/ui/managers/FontManager.h"
#include "framework/core/Dispatcher.h"
#include <cstdio>

MusicActivity::MusicActivity(ActivityManager* manager, std::string path)
    : Activity(manager), musicPath_(std::move(path)) {}

// 析构函数
MusicActivity::~MusicActivity() {}

void MusicActivity::onCreate()
{
    LV_LOG_USER("MusicActivity::onCreate");
    // swipeToReturnEnabled = false

    view = std::make_unique<MusicPlayerView>(root);

}

void MusicActivity::onResume()
{
    LV_LOG_USER("MusicActivity::onResume");
    // 使用全局 AudioService 控制播放
    auto& audio = AudioService::getInstance();
    // 若是同一曲目则不重载源、不改变播放状态，仅订阅监听刷新 UI
    if (audio.getCurrentSource() != musicPath_) {
        audio.setSource(musicPath_);
        audio.setVolume(0.4f);
        audio.play();
    }

    // 订阅进度更新并刷新 UI（Slider 与时间标签）
    progressListenerId_ = audio.addProgressListener([this](int64_t posMs, int64_t durMs){
        // 安全检查
        if (!view) return; 
        if (durMs > 0) {
            if (lastDurationMs_ != durMs) {
                lv_slider_set_range(view->SeekBar, 0, durMs);
                lastDurationMs_ = durMs;
            }
            lv_slider_set_value(view->SeekBar, posMs, LV_ANIM_OFF);
        }
        int totalSecPos = static_cast<int>(posMs / 1000);
        lv_label_set_text_fmt(view->CurrentTime, "%d:%02d", 
                              totalSecPos / 60, 
                              totalSecPos % 60);

        if (durMs > 0 && posMs >= 0) {
            // 使用总秒数减去当前秒数，保证两边变化同步
            int totalSecDur = static_cast<int>(durMs / 1000);
            int totalSecRem = totalSecDur - totalSecPos;
            
            lv_label_set_text_fmt(view->RemainingTime, "-%d:%02d", 
                                  totalSecRem / 60, 
                                  totalSecRem % 60);
        } else {
            lv_label_set_text(view->RemainingTime, "- --:--");
        }
    });

    // 绑定 Slider 事件：松开时进行 seek（避免频繁 seek）
    lv_obj_add_event_cb(view->SeekBar, [](lv_event_t* e){
        if (lv_event_get_code(e) != LV_EVENT_RELEASED) return;
        auto* activity = static_cast<MusicActivity*>(lv_event_get_user_data(e));
        if (!activity) return;
        auto& audioSvc = AudioService::getInstance();
        int32_t newPos = lv_slider_get_value(activity->view->SeekBar);
        audioSvc.seekMs(newPos);
    }, LV_EVENT_RELEASED, this);

    // 绑定 播放/暂停 按钮事件
    lv_obj_add_event_cb(view->ButtonPlay, [](lv_event_t* e){
        if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
        auto& audioSvc = AudioService::getInstance();
        if (audioSvc.getState() == PlaybackState::Playing) {
            audioSvc.pause();
        } else {
            audioSvc.play();
        }
    }, LV_EVENT_CLICKED, this);

    // 监听播放状态变化，自动更新播放按钮图标
    stateListenerId_ = AudioService::getInstance().addStateListener([this](PlaybackState st){
        if (!view) return;
        if (st == PlaybackState::Playing) {
            lv_obj_set_style_bg_image_src(view->ButtonPlay, "L:/usr/share/myapp/assets/images/music/pause_76dp_FFFFFF.png", 0);
        } else {
            lv_obj_set_style_bg_image_src(view->ButtonPlay, "L:/usr/share/myapp/assets/images/music/play_76dp_FFFFFF.png", 0);
        }
    });

    // 根据当前状态刷新一次按钮图标，保证初始一致性
    if (view) {
        auto st = audio.getState();
        if (st == PlaybackState::Playing) {
            lv_obj_set_style_bg_image_src(view->ButtonPlay, "L:/usr/share/myapp/assets/images/music/pause_76dp_FFFFFF.png", 0);
        } else {
            lv_obj_set_style_bg_image_src(view->ButtonPlay, "L:/usr/share/myapp/assets/images/music/play_76dp_FFFFFF.png", 0);
        }
    }

    // 监听元数据，更新歌名和歌手
    metadataListenerId_ = AudioService::getInstance().addMetadataListener([this](const std::string& title, const std::string& artist){
        if (!view) return;
        if (!title.empty()) lv_label_set_text(view->SongTitle, title.c_str());
        if (!artist.empty()) lv_label_set_text(view->Artist, artist.c_str());
    });

    printf("[MusicActivity] UI: onResume 完成，已添加监听器 (this=%p, progressID=%d)\n", this, progressListenerId_);
}

void MusicActivity::onDestroy()
{
    printf("[MusicActivity] UI: onDestroy 开始，即将移除监听器 (this=%p, progressID=%d)\n", this, progressListenerId_);
    // 注销进度监听，避免页面销毁后仍更新 UI
    if (progressListenerId_ != 0) {
        AudioService::getInstance().removeProgressListener(progressListenerId_);
        progressListenerId_ = 0;
    }
    if (stateListenerId_ != 0) {
        AudioService::getInstance().removeStateListener(stateListenerId_);
        stateListenerId_ = 0;
    }
    if (metadataListenerId_ != 0) {
        AudioService::getInstance().removeMetadataListener(metadataListenerId_);
        metadataListenerId_ = 0;
    }
    printf("[MusicActivity] UI: onDestroy 完成 (this=%p)。对象即将被销毁。\n", this);
}
