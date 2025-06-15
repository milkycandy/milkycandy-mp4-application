#include "Activity.h"
#include "lvgl.h"

Activity::Activity(ActivityManager* manager)
    : manager(manager), root(nullptr), swipeToReturnEnabled(true) {
}

void Activity::onResume() {
    // 默认实现为空，子类可以重写
}

void Activity::onPause() {
    // 默认实现为空，子类可以重写
}

void Activity::onDestroy() {
    // 默认实现为空，子类可以重写
}

void Activity::create() {
    // 创建一个全屏的根对象
    root = lv_obj_create(lv_screen_active());

    // --- 正确的样式处理方式 ---
    // 我们不移除所有样式，而是精确地覆盖掉我们不想要的默认主题样式属性。
    // lv_obj_remove_style_all(root);  // <--- 不要使用这个

    // lv_obj_clear_flag(root, LV_OBJ_FLAG_CLICKABLE);

    // 将我们不想要的装饰属性设置为0
    lv_obj_set_style_radius(root, 0, 0);      // 移除圆角
    lv_obj_set_style_border_width(root, 0, 0);  // 移除边框
    lv_obj_set_style_pad_all(root, 0, 0);     // 移除所有方向的内边距

    // 设置尺寸和位置
    lv_obj_set_size(root, lv_obj_get_width(lv_screen_active()), lv_obj_get_height(lv_screen_active()));
    lv_obj_set_pos(root, 0, 0);

    // 调用子类重写的onCreate来构建UI (例如设置背景色)
    // 此时，背景色会应用在没有圆角和边框的容器上
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
