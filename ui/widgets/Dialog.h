#pragma once

#include <functional>
#include <string>

#include "../ActivityManager.h"
#include "lvgl.h"

class Dialog {
   public:
    /**
     * @brief 显示一个只有一个按钮的对话框（如“确定”）。
     *
     * 此对话框常用于简单提示类操作，点击按钮后触发回调函数。
     *
     * @param title 对话框标题，默认为 "提示"
     * @param text  对话框正文内容，默认为空
     * @param positive_cb 点击“确定”按钮后的回调函数，可为空
     * @param positive_text “确定”按钮的文本，默认为 "确定"
     */
    static void showSingleButton(const std::string &title = "提示", const std::string &text = "",
                                 const std::function<void()> &positive_cb = nullptr,
                                 const std::string &positive_text = "确定");

    /**
     * @brief 显示一个带有“确定”和“取消”两个按钮的对话框。
     *
     * 适用于需要用户确认或取消操作的场景。
     *
     * @param title 对话框标题，默认为 "请确认"
     * @param text 对话框正文内容，默认为空
     * @param positive_cb 点击“确定”按钮后的回调函数，可为空
     * @param negative_cb 点击“取消”按钮或遮罩区域后的回调函数，可为空
     * @param positive_text “确定”按钮的文本，默认为 "确定"
     * @param negative_text “取消”按钮的文本，默认为 "取消"
     */
    static void showDualButton(const std::string &title = "请确认", const std::string &text = "",
                               const std::function<void()> &positive_cb = nullptr,
                               const std::function<void()> &negative_cb = nullptr,
                               const std::string &positive_text = "确定", const std::string &negative_text = "取消");

   private:
    Dialog(const std::string &title, const std::string &text, bool is_dual_button,
           const std::function<void()> &positive_cb, const std::function<void()> &negative_cb,
           const std::string &positive_text, const std::string &negative_text);

    void create_ui(lv_obj_t *parent, const std::string &title, const std::string &text, bool is_dual_button,
                   const std::string &positive_text, const std::string &negative_text);

    ~Dialog() = default;
    void show_animation();
    void close_animation();
    static void button_event_cb(lv_event_t *e);
    static void animation_finish_delete_cb(lv_anim_t *a);
    static void mask_event_cb(lv_event_t *e);
    lv_obj_t *btn_positive_ = nullptr;
    lv_obj_t *btn_negative_ = nullptr;
    lv_coord_t anim_start_y_ = 0;
    lv_coord_t anim_end_y_ = 0;
    lv_obj_t *container_ = nullptr;
    lv_obj_t *mask_ = nullptr;
    lv_obj_t *alert_dialog_ = nullptr;
    std::function<void()> positive_callback_;
    std::function<void()> negative_callback_;
};