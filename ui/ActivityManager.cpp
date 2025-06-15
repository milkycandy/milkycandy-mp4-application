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
        addSwipeGesture(newTop);
    }
}

void ActivityManager::afterStartAnimation() {
    if (activityStack.empty()) return;
    Activity* currentTop = activityStack.back();
    currentTop->onResume();
    addSwipeGesture(currentTop);
}

void ActivityManager::addSwipeGesture(Activity* activity) {
    if (activity->swipeToReturnEnabled && activityStack.size() > 1) {
        lv_obj_add_event_cb(activity->root, swipe_event_cb, LV_EVENT_ALL, this);
    }
}


// --- 动画回调函数已被完全删除 ---


void ActivityManager::swipe_event_cb(lv_event_t* e) {
    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    lv_event_code_t code = lv_event_get_code(e);
    auto* manager = static_cast<ActivityManager*>(lv_event_get_user_data(e));

    lv_indev_t* indev = lv_indev_get_act();
    if (!indev || !manager) return;

    static lv_point_t start_pos;
    static bool is_swiping = false;

    if (code == LV_EVENT_PRESSED) {
        lv_indev_get_point(indev, &start_pos);
        is_swiping = false;
    } else if (code == LV_EVENT_PRESSING) {
        lv_point_t current_pos;
        lv_indev_get_point(indev, &current_pos);
        lv_coord_t diff_x = current_pos.x - start_pos.x;
        lv_coord_t diff_y = current_pos.y - start_pos.y;

        if (!is_swiping && diff_x > 10 && std::abs(diff_x) > std::abs(diff_y)) {
            if (manager->activityStack.size() > 1) {
                Activity* prev_activity = manager->activityStack[manager->activityStack.size() - 2];
                lv_obj_clear_flag(prev_activity->root, LV_OBJ_FLAG_HIDDEN);
            }
            is_swiping = true;
        }

        if (is_swiping) {
            // 只有当手指向右滑动时才移动，但允许滑回到0的位置
            // 防止界面被向左拖动到屏幕外
            lv_obj_set_x(target, diff_x > 0 ? diff_x : 0);
        }
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        if (!is_swiping) return;
        is_swiping = false;

        lv_coord_t current_x = lv_obj_get_x(target);
        lv_coord_t swipe_threshold = lv_obj_get_width(lv_screen_active()) / 3;

        // --- 动画移除 ---
        if (current_x > swipe_threshold) {
            // 成功返回：之前是动画滑出，现在直接调用finish
            manager->finishCurrentActivity();
        } else {
            // 取消返回：之前是动画弹回，现在直接设置x坐标为0
            lv_obj_set_x(target, 0);

            // 之前在弹回动画结束后隐藏下层页面，现在直接操作
            if (manager->activityStack.size() > 1) {
                Activity* prev_activity = manager->activityStack[manager->activityStack.size() - 2];
                lv_obj_add_flag(prev_activity->root, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
}
