#pragma once
#include "lvgl.h"
#include <string>

class Toast
{
public:
    /**
     * @brief Toast 显示时长枚举。
     *
     * 用于指定 Toast 显示的时间长度。
     */
    enum Duration {
        SHORT = 2000,
        LONG = 3500
    };

    /**
     * @brief 显示一个带动画的 Toast 消息。
     *
     * 该 Toast 会自动淡入、显示指定时长后淡出并销毁。用于短暂提示用户。
     *
     * @param text 要显示的文本内容
     * @param duration 显示时长（可不填，默认为SHORT），SHORT 或 LONG
     * SHORT 为 2 秒，LONG 为 3.5 秒
     */
    static void show(const std::string &text, Duration duration = SHORT);

private:
    static void fade_in(lv_obj_t *obj);
    static void fade_out_cb(lv_timer_t *timer);
};
