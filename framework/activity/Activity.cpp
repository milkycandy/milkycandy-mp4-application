#include "Activity.h"
#include "ActivityManager.h"
#include "lvgl.h"

Activity::Activity(ActivityManager* manager)
    : manager(manager) {
}

Activity::~Activity() {
    // 释放 LVGL 根对象
    if (root) {
        lv_obj_delete(root);
        root = nullptr;
    }
}

void Activity::onResume() {
    // 默认空实现
}

void Activity::onPause() {
    // 默认空实现
}

void Activity::onDestroy() {
    // 默认空实现
}

void Activity::internal_create() {
    // 创建全屏根容器
    root = lv_obj_create(lv_screen_active());

    // 移除默认样式的装饰（圆角、边框、内边距），确保是纯净的全屏容器
    lv_obj_set_style_radius(root, 0, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);

    // 强制几何属性
    lv_obj_set_size(root, lv_obj_get_width(lv_screen_active()), lv_obj_get_height(lv_screen_active()));
    lv_obj_set_pos(root, 0, 0);

    // 触发用户构建逻辑
    onCreate();
}