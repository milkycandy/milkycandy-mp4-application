#include "ActivityManager.h"
#include "lvgl.h"
#include <cmath>

namespace {
    // 动画完成回调：销毁一个 activity。
    // 当“消失”动画播放完毕时调用此函数。
    void anim_ready_cb_destroy_activity(lv_anim_t* a) {
        Activity* activity = static_cast<Activity*>(a->user_data);
        if (activity) {
            activity->destroy();
            delete activity;
        }
    }

    // 动画完成回调：恢复一个 activity。
    // 当新 activity 的“出现”动画播放完毕时调用此函数。
    void anim_ready_cb_resume_activity(lv_anim_t* a) {
        Activity* activity = static_cast<Activity*>(a->user_data);
        if (activity) {
            activity->onResume();
        }
    }

    // 动画完成回调：隐藏一个 lv_obj_t 对象。
    // 用于在新 activity 入场动画播放完毕后，隐藏上一个 activity。
    void anim_ready_cb_hide_obj(lv_anim_t* a) {
        lv_obj_t* obj = static_cast<lv_obj_t*>(a->user_data);
        if (obj) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

ActivityManager& ActivityManager::getInstance() {
    static ActivityManager instance;
    return instance;
}

ActivityManager::~ActivityManager() {
    for (auto activity : activityStack) {
        delete activity;
    }
    activityStack.clear();
}

void ActivityManager::startActivity(Activity* newActivity) {
    Activity* currentTop = nullptr;
    if (!activityStack.empty()) {
        currentTop = activityStack.back();
        currentTop->onPause();
    }

    newActivity->create();
    activityStack.push_back(newActivity);

    // 如果启用了动画，则设置“出现”动画。
    if (newActivity->enableEnterAnimation) {
        // 新 activity 的入场动画完成回调将负责恢复其运行。
        appear_Animation(newActivity->root, anim_ready_cb_resume_activity, newActivity, lv_obj_get_width(lv_screen_active()));
        
        if (currentTop) {
            // 我们使用一个并行的、不可见的“哑”动画（dummy animation），
            // 仅仅是为了利用它的完成回调，在相同的时间后隐藏上一个 activity。
            // 这是一个无需另外创建 lv_timer 就能确保同步的简洁方法。
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, currentTop->root);
            lv_anim_set_time(&a, ANIMATION_DURATION);
            lv_anim_set_values(&a, 0, 0); // 一个不做任何事情的动画
            lv_anim_set_ready_cb(&a, anim_ready_cb_hide_obj);
            lv_anim_set_user_data(&a, currentTop->root);
            lv_anim_start(&a);
        }

    } else {
        // 如果没有动画，直接将其放置到位并调用 onResume。
        lv_obj_set_x(newActivity->root, 0);
        newActivity->onResume();
        if (currentTop) {
            lv_obj_add_flag(currentTop->root, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ActivityManager::finishCurrentActivity() {
    if (activityStack.size() < 2) return; // 不能关闭最后一个 activity

    Activity* activityToFinish = activityStack.back();
    activityStack.pop_back();

    activityToFinish->onPause();
    
    // 恢复新的栈顶 activity
    Activity* newTop = activityStack.back();
    lv_obj_clear_flag(newTop->root, LV_OBJ_FLAG_HIDDEN);
    newTop->onResume();
    
    // 播放“消失”动画，并设置其完成回调来销毁被关闭的 activity。
    disappear_Animation(activityToFinish->root, anim_ready_cb_destroy_activity, activityToFinish);
}


void ActivityManager::enableGlobalSwipe() {
    lv_timer_create([](lv_timer_t*) {
        static lv_point_t start_point;
        static bool pressed_last = false;
        static bool is_swiping = false;

        auto& manager = ActivityManager::getInstance();
        if (manager.activityStack.size() <= 1) return;

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
            if (!pressed_last) { // 按下开始
                start_point = point;
                is_swiping = false;
            }

            lv_coord_t dx = point.x - start_point.x;
            lv_coord_t dy = std::abs(point.y - start_point.y);

            // 开始滑动收拾
            if (!is_swiping && dx > 20 && dx > dy) {
                is_swiping = true;
                // 取消当前 activity 上所有正在播放的动画（例如，上一次未完成的“回弹”动画）
                lv_anim_del(current->root, nullptr); 
                
                // 显示下方的上一个 activity
                Activity* prev = manager.activityStack[manager.activityStack.size() - 2];
                
                // 【修复】关键修复点：
                // 同时取消即将隐藏上一个 activity 的“哑”动画，防止它在之后错误地触发。
                lv_anim_del(prev->root, nullptr);

                lv_obj_clear_flag(prev->root, LV_OBJ_FLAG_HIDDEN);
            }

            if (is_swiping) {
                // 跟随手指移动
                lv_obj_set_x(current->root, dx);
            }

        } else { // 松手
            if (is_swiping) {
                Activity* current_act = manager.activityStack.back();
                lv_coord_t current_x = lv_obj_get_x(current_act->root);
                lv_coord_t screen_width = lv_obj_get_width(lv_screen_active());
                lv_coord_t threshold = screen_width / 3;

                if (current_x > threshold) {
                    // --- 滑动成功，关闭 activity ---
                    manager.activityStack.pop_back();

                    // 播放出场动画，并在完成后销毁
                    manager.disappear_Animation(current_act->root, anim_ready_cb_destroy_activity, current_act);

                    // 恢复上一个 activity
                    if (!manager.activityStack.empty()) {
                        Activity* prev = manager.activityStack.back();
                        prev->onResume();
                    }
                } else {
                    // --- 滑动失败，弹回原位 ---
                    Activity* prev = manager.activityStack[manager.activityStack.size() - 2];
                    
                    // 将当前 activity 动画恢复到 x=0 的位置，并在动画结束后隐藏上一个 activity
                    manager.appear_Animation(current_act->root, anim_ready_cb_hide_obj, prev->root, current_x);
                }
            }
            is_swiping = false;
        }
        pressed_last = (state == LV_INDEV_STATE_PRESSED);
    }, 16, NULL);
}

void ActivityManager::appear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data, int startPosition) {
    // 首先停止此对象上所有其他动画
    lv_anim_del(target, nullptr);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, target);
    lv_anim_set_time(&a, ANIMATION_DURATION);
    lv_anim_set_delay(&a, 0);
    lv_anim_set_values(&a, startPosition, 0);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a, [](void* obj, int32_t v) {
        lv_obj_set_x(static_cast<lv_obj_t*>(obj), v);
    });

    if (ready_cb) {
        lv_anim_set_ready_cb(&a, ready_cb);
        lv_anim_set_user_data(&a, user_data);
    }
    
    lv_anim_start(&a);
}

void ActivityManager::disappear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data) {
    // 首先停止此对象上所有其他动画
    lv_anim_del(target, nullptr);

    lv_coord_t screen_width = lv_obj_get_width(lv_screen_active());
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, target);
    lv_anim_set_time(&a, ANIMATION_DURATION);
    lv_anim_set_delay(&a, 0);
    lv_anim_set_values(&a, lv_obj_get_x(target), screen_width);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a, [](void* obj, int32_t v) {
        lv_obj_set_x(static_cast<lv_obj_t*>(obj), v);
    });

    if (ready_cb) {
        lv_anim_set_ready_cb(&a, ready_cb);
        lv_anim_set_user_data(&a, user_data);
    }

    lv_anim_start(&a);
}
