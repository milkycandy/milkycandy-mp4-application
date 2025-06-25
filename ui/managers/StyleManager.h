#pragma once
#include "lvgl.h"

class StyleManager {
public:
    // 初始化样式（程序启动时调用）
    static void init();

    // 样式获取函数
    static lv_style_t* getHomepageStyle();
    static lv_style_t* getTileContainerStyle();
    static lv_style_t* getPressedImageButtonStyle();
    static lv_style_t* getTileLabelStyle();

private:
    static lv_style_t homepageStyle;
    static lv_style_t tileContainerStyle;
    static lv_style_t pressedImageButtonStyle;
    static lv_style_t tileLabelStyle;
};
