#include "MusicActivity.h"
#include "ActivityManager.h"
#include "widgets/Dialog.h"
#include "widgets/Toast.h"
#include "managers/FontManager.h"
#include "lvgl.h"

void MusicActivity::onCreate()
{
    // swipeToReturnEnabled = false;

    lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_GREEN), 0);

    lv_obj_t *label = lv_label_create(root);
    lv_label_set_text(label, "我是新页面");
    lv_obj_set_style_text_font(label, FontManager::get_font(28), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t *btn = lv_btn_create(root);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, back_button_event_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_obj_set_style_text_font(btn_label, FontManager::get_font(28), 0);
    lv_label_set_text(btn_label, "显示Dialog");
    lv_obj_center(btn_label);
}

void MusicActivity::onDestroy()
{
    LV_LOG_USER("MusicActivity onDestroy");
}

void MusicActivity::back_button_event_cb(lv_event_t *e)
{
    // ActivityManager::getInstance().finishCurrentActivity();
    Dialog::showDualButton(
        "你好",
        "早上好！\n我是一个双按钮Dialog! 你想见见单按钮Dialog吗？",
        []()
        {
            Toast::show("来了！");
            Dialog::showSingleButton(
                "Hi",
                "我是单按钮Dialog，向你问好！",
                []()
                {
                    Toast::show("下次见！");
                });
        },
        []()
        {
            Toast::show("那拜拜！");
        },
        "好捏",
        "不要");
}
