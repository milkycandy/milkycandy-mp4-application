#pragma once

#include "lvgl.h"
#include <string>
#include <functional>
#include "../ActivityManager.h"

class Dialog {
public:
    static void showSingleButton(
        const std::string& title = "提示",
        const std::string& text = "",
        const std::function<void()>& positive_cb = nullptr,
        const std::string& positive_text = "确定"
    );

    static void showDualButton(
        const std::string& title = "请确认",
        const std::string& text = "",
        const std::function<void()>& positive_cb = nullptr,
        const std::function<void()>& negative_cb = nullptr,
        const std::string& positive_text = "确定",
        const std::string& negative_text = "取消"
    );

private:
    Dialog(
        const std::string& title,
        const std::string& text,
        bool is_dual_button,
        const std::function<void()>& positive_cb,
        const std::function<void()>& negative_cb,
        const std::string& positive_text,
        const std::string& negative_text
    );
    
    void create_ui(
        lv_obj_t* parent,
        const std::string& title,
        const std::string& text,
        bool is_dual_button,
        const std::string& positive_text,
        const std::string& negative_text
    );

    ~Dialog() = default;
    void show_animation();
    void close_animation();
    static void button_event_cb(lv_event_t* e);
    static void animation_finish_delete_cb(lv_anim_t* a);
    static void mask_event_cb(lv_event_t* e);
    lv_obj_t* btn_positive_ = nullptr;
    lv_obj_t* btn_negative_ = nullptr;
    lv_coord_t anim_start_y_ = 0;
    lv_coord_t anim_end_y_ = 0;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* mask_ = nullptr;
    lv_obj_t* alert_dialog_ = nullptr;
    std::function<void()> positive_callback_;
    std::function<void()> negative_callback_;
};