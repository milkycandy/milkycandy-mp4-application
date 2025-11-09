#include "MusicActivity.h"
#include "framework/activity/ActivityManager.h"
#include "framework/ui/widgets/Dialog.h"
#include "framework/ui/widgets/Toast.h"
#include "framework/ui/managers/FontManager.h"
#include "lvgl.h"

MusicActivity::MusicActivity(ActivityManager* manager, std::string path)
    : Activity(manager), musicPath_(std::move(path)) {}

// 析构函数
MusicActivity::~MusicActivity() {}

void MusicActivity::onCreate()
{
    // swipeToReturnEnabled = false;

    lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_GREEN), 0);

    lv_obj_t *label = lv_label_create(root);
    lv_label_set_text(label, musicPath_.c_str());
    lv_obj_set_style_text_font(label, FontManager::getFont(28), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);

    player_ = std::make_unique<GStreamerAudioPlayer>();
    player_->setSource(musicPath_);
    player_->setVolume(0.80f);
    player_->setOnEos([](){
        // 播放结束可做提示或自动退出
        Toast::show("播放结束");
    });
    player_->setOnError([](const std::string& reason){
        Toast::show("播放失败: " + reason);
        LV_LOG_ERROR("GStreamerAudioPlayer error: %s", reason.c_str());
    });
    player_->play();
}

void MusicActivity::onDestroy()
{
}
