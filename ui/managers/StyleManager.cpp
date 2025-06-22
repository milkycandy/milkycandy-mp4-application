#include "StyleManager.h"
#include "../managers/FontManager.h"

// 静态成员定义
lv_style_t StyleManager::defaultTextStyle;
lv_style_t StyleManager::centeredTextStyle;
lv_style_t StyleManager::highlightedTextStyle;

void StyleManager::init() {
    FontManager::init();
    // 默认样式
    lv_style_init(&defaultTextStyle);
    lv_style_set_text_font(&defaultTextStyle, FontManager::get_font(28));
    lv_style_set_text_color(&defaultTextStyle, lv_color_black());

    // 居中文本
    lv_style_init(&centeredTextStyle);
    lv_style_set_text_font(&centeredTextStyle, FontManager::get_font(28));
    lv_style_set_text_align(&centeredTextStyle, LV_TEXT_ALIGN_CENTER);

    // 高亮样式
    lv_style_init(&highlightedTextStyle);
    lv_style_set_text_font(&highlightedTextStyle, FontManager::get_font(28));
    lv_style_set_text_color(&highlightedTextStyle, lv_color_hex(0xff6600));
    lv_style_set_text_decor(&highlightedTextStyle, LV_TEXT_DECOR_UNDERLINE);
}

lv_style_t* StyleManager::get_default_text_style() {
    return &defaultTextStyle;
}

lv_style_t* StyleManager::get_centered_text_style() {
    return &centeredTextStyle;
}

lv_style_t* StyleManager::get_highlighted_text_style() {
    return &highlightedTextStyle;
}
