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

    static lv_style_t* getFileListItemStyle(bool is_odd); // is_odd 用于判断奇偶行
    static lv_style_t* getFileListLabelStyle();

private:
    static lv_style_t homepageStyle;
    static lv_style_t tileContainerStyle;
    static lv_style_t pressedImageButtonStyle;
    static lv_style_t tileLabelStyle;

    static lv_style_t fileListItemStyle_Odd;   // 奇数行样式 (白)
    static lv_style_t fileListItemStyle_Even;  // 偶数行样式 (灰)
    static lv_style_t fileListLabelStyle;
};
