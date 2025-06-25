#include "StyleManager.h"
#include "../managers/FontManager.h"

// 样式定义
lv_style_t StyleManager::homepageStyle;
lv_style_t StyleManager::tileContainerStyle;
lv_style_t StyleManager::pressedImageButtonStyle;
lv_style_t StyleManager::tileLabelStyle;

void StyleManager::init() {
    FontManager::init();

    // 主页容器样式
    lv_style_init(&homepageStyle);
    lv_style_set_pad_left(&homepageStyle, 0);
    lv_style_set_pad_right(&homepageStyle, 0);
    lv_style_set_pad_top(&homepageStyle, 56);
    lv_style_set_pad_bottom(&homepageStyle, 0);
    lv_style_set_pad_row(&homepageStyle, 16);
    lv_style_set_pad_column(&homepageStyle, 0);
    lv_style_set_bg_opa(&homepageStyle, LV_OPA_TRANSP);

    // 子项容器样式（音乐/视频/设置）
    lv_style_init(&tileContainerStyle);
    lv_style_set_width(&tileContainerStyle, 192);
    lv_style_set_height(&tileContainerStyle, 230);

    // 按钮按下变暗样式
    lv_style_init(&pressedImageButtonStyle);
    lv_style_set_image_recolor(&pressedImageButtonStyle, lv_color_hex(0x000000));
    lv_style_set_image_recolor_opa(&pressedImageButtonStyle, 80);

    // 标签样式
    lv_style_init(&tileLabelStyle);
    lv_style_set_text_color(&tileLabelStyle, lv_color_hex(0xFFFFFF));
    lv_style_set_text_opa(&tileLabelStyle, 255);
    lv_style_set_text_font(&tileLabelStyle, FontManager::getFont(28));
}

lv_style_t* StyleManager::getHomepageStyle() {
    return &homepageStyle;
}

lv_style_t* StyleManager::getTileContainerStyle() {
    return &tileContainerStyle;
}

lv_style_t* StyleManager::getPressedImageButtonStyle() {
    return &pressedImageButtonStyle;
}

lv_style_t* StyleManager::getTileLabelStyle() {
    return &tileLabelStyle;
}
