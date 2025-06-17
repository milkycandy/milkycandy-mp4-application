#pragma once
#include "lvgl.h"
#include <string>

class Toast {
public:
    enum Duration {
        SHORT = 2000,
        LONG = 3500
    };

    static void show(const std::string& text, Duration duration = SHORT);

private:
    static void fade_in(lv_obj_t* obj);
    static void fade_out_cb(lv_timer_t* timer);
};
