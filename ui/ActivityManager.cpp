#include "ActivityManager.h"
#include "lvgl.h"
#include <cmath>

ActivityManager& ActivityManager::getInstance() {
    static ActivityManager instance;
    return instance;
}

ActivityManager::~ActivityManager() {
    for (auto activity : activityStack) {
        // activity->destroy();
        delete activity;
    }
    activityStack.clear();
}

void ActivityManager::startActivity(Activity* newActivity) {
    if (!activityStack.empty()) {
        Activity* currentTop = activityStack.back();
        currentTop->onPause();
        lv_obj_add_flag(currentTop->root, LV_OBJ_FLAG_HIDDEN);
    }

    newActivity->create();
    activityStack.push_back(newActivity);

    // --- 动画移除 ---
    // 之前这里是入场动画，现在直接设置到最终位置
    lv_obj_set_x(newActivity->root, 0);

    // 之前在动画结束后调用的函数，现在直接同步调用
    afterStartAnimation();
}

void ActivityManager::finishCurrentActivity() {
    if (activityStack.empty()) return;

    Activity* activityToFinish = activityStack.back();
    activityStack.pop_back();

    activityToFinish->onPause();
    activityToFinish->destroy();
    delete activityToFinish;

    if (!activityStack.empty()) {
        Activity* newTop = activityStack.back();
        lv_obj_clear_flag(newTop->root, LV_OBJ_FLAG_HIDDEN);
        newTop->onResume();
    }
}

void ActivityManager::afterStartAnimation() {
    if (activityStack.empty()) return; // 一般来讲不可能是空的，因为HomeActivity关不掉
    Activity* currentTop = activityStack.back();
    currentTop->onResume();
}

void ActivityManager::enableGlobalSwipe() {
    lv_timer_create([](lv_timer_t*) {
        static lv_point_t start_point;
        static bool pressed_last = false;
        static bool is_swiping = false;
        static bool started = false;

        ActivityManager& manager = ActivityManager::getInstance();
        Activity* current = manager.activityStack.back();
        if (!current->swipeToReturnEnabled) return;

        lv_indev_t * indev = lv_indev_get_next(NULL);
        while (indev) {
            if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) break;
            indev = lv_indev_get_next(indev);
        }
        if (!indev) return;

        lv_point_t point;
        lv_indev_get_point(indev, &point);
        lv_indev_state_t state = lv_indev_get_state(indev);

        if (state == LV_INDEV_STATE_PRESSED) {
            if (!pressed_last) {
                start_point = point;
                started = false;
            }

            lv_coord_t dx = point.x - start_point.x;
            lv_coord_t dy = point.y - start_point.y;

            if (!started && dx > 10 && std::abs(dx) > std::abs(dy)) {
                if (manager.activityStack.size() > 1) {
                    Activity* prev = manager.activityStack[manager.activityStack.size() - 2];
                    lv_obj_clear_flag(prev->root, LV_OBJ_FLAG_HIDDEN);
                    is_swiping = true;
                    started = true;
                }
            }

            if (is_swiping) {
                Activity* current = manager.activityStack.back();
                lv_obj_set_x(current->root, dx > 0 ? dx : 0);
            }

        } else {
            if (is_swiping) {
                // 松手，开始种菜
                Activity* current = manager.activityStack.back();
                lv_coord_t x = lv_obj_get_x(current->root);
                lv_coord_t threshold = lv_obj_get_width(lv_screen_active()) / 3;

                if (x > threshold) {
                    manager.finishCurrentActivity();
                } else {
                    lv_obj_set_x(current->root, 0);
                    if (manager.activityStack.size() > 1) {
                        Activity* prev = manager.activityStack[manager.activityStack.size() - 2];
                        lv_obj_add_flag(prev->root, LV_OBJ_FLAG_HIDDEN);
                    }
                }
            }
            is_swiping = false;
        }

        pressed_last = (state == LV_INDEV_STATE_PRESSED);

    }, 16, NULL); // ~60fps
}
