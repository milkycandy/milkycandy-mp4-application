#include "MusicActivity.h"
#include "ActivityManager.h"
#include "lvgl.h"

void MusicActivity::onCreate() {
    // 可以在这里设置是否允许滑动返回
    // swipeToReturnEnabled = false;

    lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_GREEN), 0);

    lv_obj_t* label = lv_label_create(root);
    lv_label_set_text(label, "Music Player Activity");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t* btn = lv_btn_create(root);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_add_event_cb(btn, back_button_event_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, LV_SYMBOL_LEFT " Back");
    lv_obj_center(btn_label);
}

void MusicActivity::onDestroy() {
    LV_LOG_USER("MusicActivity onDestroy");
}

void MusicActivity::back_button_event_cb(lv_event_t* e) {
    ActivityManager::getInstance().finishCurrentActivity();
}
