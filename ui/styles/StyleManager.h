#pragma once
#include "lvgl.h"

class StyleManager {
public:
    // 初始化样式（程序启动时调用）
    static void init(void);

    // 获取统一样式
    static lv_style_t* get_default_text_style();
    static lv_style_t* get_centered_text_style();
    static lv_style_t* get_highlighted_text_style();

private:
    static lv_style_t defaultTextStyle;
    static lv_style_t centeredTextStyle;
    static lv_style_t highlightedTextStyle;
};
