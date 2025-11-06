#include <stdio.h>
#include <time.h>

#include "StatusBarManager.h"
#include "FontManager.h"

LV_IMG_DECLARE(statusbar_bluetooth);
LV_IMG_DECLARE(statusbar_battery_70);

StatusBarManager& StatusBarManager::getInstance() {
    static StatusBarManager instance;
    return instance;
}

void StatusBarManager::init() {
    ui_StatusBar = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(ui_StatusBar);
    lv_obj_set_width(ui_StatusBar, lv_pct(100));
    lv_obj_set_height(ui_StatusBar, lv_pct(6));
    lv_obj_set_align(ui_StatusBar, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_StatusBar, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));

    ui_ImageBluetooth = lv_image_create(ui_StatusBar);
    lv_image_set_src(ui_ImageBluetooth, &statusbar_bluetooth);
    lv_obj_set_width(ui_ImageBluetooth, LV_SIZE_CONTENT);   /// 40
    lv_obj_set_height(ui_ImageBluetooth, LV_SIZE_CONTENT);  /// 40
    lv_obj_set_x(ui_ImageBluetooth, 3);
    lv_obj_set_y(ui_ImageBluetooth, 0);
    lv_obj_set_align(ui_ImageBluetooth, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ImageBluetooth, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_ImageBluetooth, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_image_recolor(ui_ImageBluetooth, lv_color_hex(0x267BFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(ui_ImageBluetooth, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelTime = lv_label_create(ui_StatusBar);
    lv_obj_set_width(ui_LabelTime, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LabelTime, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_LabelTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelTime, "3:10 PM");
    lv_obj_set_style_text_font(ui_LabelTime, FontManager::getFont(28), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ImageBattery = lv_image_create(ui_StatusBar);
    lv_image_set_src(ui_ImageBattery, &statusbar_battery_70);
    lv_obj_set_width(ui_ImageBattery, LV_SIZE_CONTENT);   /// 28
    lv_obj_set_height(ui_ImageBattery, LV_SIZE_CONTENT);  /// 28
    lv_obj_set_x(ui_ImageBattery, -6);
    lv_obj_set_y(ui_ImageBattery, 0);
    lv_obj_set_align(ui_ImageBattery, LV_ALIGN_RIGHT_MID);
    lv_obj_add_flag(ui_ImageBattery, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_ImageBattery, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_image_recolor(ui_ImageBattery, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(ui_ImageBattery, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void StatusBarManager::show() {
    if (ui_StatusBar) {
        lv_obj_clear_flag(ui_StatusBar, LV_OBJ_FLAG_HIDDEN);
    }
}

void StatusBarManager::hide() {
    if (ui_StatusBar) {
        lv_obj_add_flag(ui_StatusBar, LV_OBJ_FLAG_HIDDEN);
    }
}