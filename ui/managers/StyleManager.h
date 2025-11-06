#pragma once
#include "lvgl.h"

class StyleManager {
public:
    /**
     * @brief 初始化所有全局UI样式。
     * @warning 在调用此函数之前，必须先调用 FontManager::init() 来加载字体资源，
     *          因为样式定义依赖于字体。
     */
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
