#include "Activity.h"
#include "lvgl.h"

Activity::Activity(ActivityManager* manager)
    : manager(manager) {
}

void Activity::onResume() {
    // 
}

void Activity::onPause() {
    // 
}

void Activity::onDestroy() {
    // 
}

void Activity::create() {
    // 创建一个全屏的根对象
    root = lv_obj_create(lv_screen_active());

    // 将不想要的装饰属性设置为0，不然默认的容器是个panel样子的东西，很奇怪
    lv_obj_set_style_radius(root, 0, 0);      // 移除圆角
    lv_obj_set_style_border_width(root, 0, 0);  // 移除边框
    lv_obj_set_style_pad_all(root, 0, 0);     // 移除所有方向的内边距

    // 设置尺寸和位置
    lv_obj_set_size(root, lv_obj_get_width(lv_screen_active()), lv_obj_get_height(lv_screen_active()));
    lv_obj_set_pos(root, 0, 0);

    // 调用子类重写的onCreate来构建UI
    onCreate();
}

void Activity::destroy() {
    // 先调用子类重写的onDestroy来释放特定资源
    onDestroy();

    // 然后安全地删除根LVGL对象
    if (root) {
        lv_obj_delete(root);
        root = nullptr;
    }
}
