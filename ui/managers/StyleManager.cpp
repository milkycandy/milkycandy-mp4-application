#include "StyleManager.h"
#include "../managers/FontManager.h"

// 样式定义
lv_style_t StyleManager::homepageStyle;
lv_style_t StyleManager::tileContainerStyle;
lv_style_t StyleManager::pressedImageButtonStyle;
lv_style_t StyleManager::tileLabelStyle;

lv_style_t StyleManager::fileListItemStyle_Odd;
lv_style_t StyleManager::fileListItemStyle_Even;
lv_style_t StyleManager::fileListLabelStyle;

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

    // 列表项通用设置
    auto init_file_list_item_style = [](lv_style_t* style) {
        lv_style_set_width(style, lv_pct(100));
        lv_style_set_height(style, LV_SIZE_CONTENT);
        lv_style_set_pad_all(style, 6);
    };

    // 奇数行样式 (白色背景)
    lv_style_init(&fileListItemStyle_Odd);
    init_file_list_item_style(&fileListItemStyle_Odd);
    lv_style_set_bg_color(&fileListItemStyle_Odd, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&fileListItemStyle_Odd, 255);

    // 偶数行样式 (灰色背景)
    lv_style_init(&fileListItemStyle_Even);
    init_file_list_item_style(&fileListItemStyle_Even);
    lv_style_set_bg_color(&fileListItemStyle_Even, lv_color_hex(0xF4F5F5));
    lv_style_set_bg_opa(&fileListItemStyle_Even, 255);

    // 文件列表中的标签样式
    lv_style_init(&fileListLabelStyle);
    lv_style_set_text_font(&fileListLabelStyle, FontManager::getFont(26));
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

lv_style_t* StyleManager::getFileListItemStyle(bool is_odd) {
    return is_odd ? &fileListItemStyle_Odd : &fileListItemStyle_Even;
}

lv_style_t* StyleManager::getFileListLabelStyle() {
    return &fileListLabelStyle;
}
