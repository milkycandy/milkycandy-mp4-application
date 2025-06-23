#include "HomeActivity.h"
#include "ActivityManager.h"
#include "MusicActivity.h"
#include "managers/StyleManager.h"
#include "widgets/Toast.h"
#include "widgets/Dialog.h"
#include "lvgl.h"

void HomeActivity::onCreate() {
    // lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_BLUE), 0);
    enableEnterAnimation = false; // 关闭进入动画

    lv_obj_t* label1 = lv_label_create(root);
    lv_label_set_text(label1, "天地玄黄，宇宙洪荒。日月盈昃，辰宿列张。The quick brown fox jumps over the lazy dog. 1234567890。こんにちは、世界！今日はいい天気ですね。富士山が見える。");
    // lv_obj_set_style_text_font(label1, &lv_font_montserrat_24, 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(label1, 480, 800);
    lv_obj_add_style(label1, StyleManager::get_default_text_style(), 0);

    lv_obj_t* btn = lv_btn_create(root);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(btn, go_to_music_event_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "打开新页面");
    lv_obj_center(btn_label);
    lv_obj_add_style(btn_label, StyleManager::get_centered_text_style(), 0);

    lv_obj_t * img = lv_image_create(root);
    lv_image_set_src(img, "S:/root/首先要选择做什么梦.png");
    lv_obj_center(img);
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
    // Toast::show("你好，我是Toast！");
    // Dialog::showDualButton(
    //     "你好",
    //     "我是一个双按钮Dialog! 你想见见单按钮Dialog吗？天地玄黄，宇宙洪荒。日月盈昃，辰宿列张。The quick brown fox jumps over the lazy dog. 1234567890。こんにちは、世界！今日はいい天気ですね。富士山が見える。",
    //     []() {
    //         Toast::show("来了！");
    //         Dialog::showSingleButton(
    //             "Hi",
    //             "我是单按钮Dialog，向你问好！",
    //              []() {
    //                 Toast::show("下次见！");
    //              }
    //         );
    //     },
    //     []() {
    //         Toast::show("那拜拜！");
    //     },
    //     "好捏",
    //     "不要"
    // );
}
