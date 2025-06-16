#include "HomeActivity.h"
#include "ActivityManager.h"
#include "MusicActivity.h"
#include "lvgl.h"

void HomeActivity::onCreate() {
    // lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_BLUE), 0);
    enableEnterAnimation = false; // 关闭进入动画

    lv_obj_t* label = lv_label_create(root);
    lv_label_set_text(label, "Home Activity");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t* btn = lv_btn_create(root);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(btn, go_to_music_event_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Go to Music Player");
    lv_obj_center(btn_label);
}

void HomeActivity::onResume() {
    LV_LOG_USER("HomeActivity onResume");
}

void HomeActivity::onPause() {
    LV_LOG_USER("HomeActivity onPause");
}

// 一般情况下这个 activity 不会出现onDestory

void HomeActivity::go_to_music_event_cb(lv_event_t* e) {
    ActivityManager& manager = ActivityManager::getInstance();
    manager.startActivity(new MusicActivity(&manager));
}
