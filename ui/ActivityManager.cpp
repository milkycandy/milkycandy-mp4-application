#include "ActivityManager.h"
#include "lvgl.h"
#include <cmath>

namespace AnimationCallbacks {
    // 动画完成回调：销毁一个 activity
    void destroyActivity(lv_anim_t* a) {
        Activity* activity = static_cast<Activity*>(a->user_data);
        if (activity) {
            activity->destroy();
            delete activity;
        }
    }

    // 动画完成回调：恢复一个 activity
    void resumeActivity(lv_anim_t* a) {
        Activity* activity = static_cast<Activity*>(a->user_data);
        if (activity) {
            activity->onResume();
        }
    }

    // 动画完成回调：隐藏一个 lv_obj_t 对象
    void hideObject(lv_anim_t* a) {
        lv_obj_t* obj = static_cast<lv_obj_t*>(a->user_data);
        if (obj) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

namespace SwipeGesture {
    struct State {
        lv_point_t startPoint;
        bool pressedLast = false;
        bool isSwiping = false;
    };
    
    static State state;
    static constexpr lv_coord_t SWIPE_START_THRESHOLD = 20;
    static constexpr int TIMER_PERIOD_MS = 16;
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
    if (!newActivity) return;

    Activity* currentTop = nullptr;
    if (!activityStack.empty()) {
        currentTop = activityStack.back();
        currentTop->onPause();
    }

    newActivity->create();
    activityStack.push_back(newActivity);

    if (newActivity->enableEnterAnimation) {
        // 新 activity 入场动画
        appear_Animation(newActivity->root, AnimationCallbacks::resumeActivity, 
                        newActivity, lv_obj_get_width(lv_screen_active()));
        
        // 同步隐藏上一个 activity 的哑动画
        if (currentTop) {
            lv_anim_t hideAnim;
            lv_anim_init(&hideAnim);
            lv_anim_set_var(&hideAnim, currentTop->root);
            lv_anim_set_time(&hideAnim, ANIMATION_DURATION);
            lv_anim_set_values(&hideAnim, 0, 0);
            lv_anim_set_ready_cb(&hideAnim, AnimationCallbacks::hideObject);
            lv_anim_set_user_data(&hideAnim, currentTop->root);
            lv_anim_start(&hideAnim);
        }
    } else {
        // 无动画直接切换
        lv_obj_set_x(newActivity->root, 0);
        newActivity->onResume();
        if (currentTop) {
            lv_obj_add_flag(currentTop->root, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ActivityManager::finishCurrentActivity() {
    if (activityStack.size() < 2) return;

    Activity* activityToFinish = activityStack.back();
    activityStack.pop_back();
    activityToFinish->onPause();
    
    // 恢复新的栈顶 activity
    Activity* newTop = activityStack.back();
    lv_obj_clear_flag(newTop->root, LV_OBJ_FLAG_HIDDEN);
    newTop->onResume();
    
    // 播放消失动画并销毁
    disappear_Animation(activityToFinish->root, AnimationCallbacks::destroyActivity, activityToFinish);
}

Activity* ActivityManager::getCurrentActivity() const {
    if (activityStack.empty()) {
        return nullptr;
    }
    return activityStack.back();
}

void ActivityManager::enableGlobalSwipe() {
    lv_timer_create([](lv_timer_t*) {
        auto& manager = ActivityManager::getInstance();
        if (manager.activityStack.size() <= 1) return;

        Activity* currentActivity = manager.activityStack.back();
        if (!currentActivity->swipeToReturnEnabled) return;

        // 获取触摸输入设备
        lv_indev_t* indev = lv_indev_get_next(nullptr);
        while (indev && lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) {
            indev = lv_indev_get_next(indev);
        }
        if (!indev) return;

        lv_point_t currentPoint;
        lv_indev_get_point(indev, &currentPoint);
        lv_indev_state_t state = lv_indev_get_state(indev);
        
        using namespace SwipeGesture;

        if (state == LV_INDEV_STATE_PRESSED) {
            if (!SwipeGesture::state.pressedLast) {
                manager.handleSwipeStart(currentPoint);
            }
            manager.handleSwipeMove(currentPoint, SwipeGesture::state.startPoint);
        } else if (SwipeGesture::state.isSwiping) {
            manager.handleSwipeEnd(SwipeGesture::state.startPoint);
        }
        
        SwipeGesture::state.pressedLast = (state == LV_INDEV_STATE_PRESSED);
    }, SwipeGesture::TIMER_PERIOD_MS, nullptr);
}

void ActivityManager::handleSwipeStart(const lv_point_t& point) {
    SwipeGesture::state.startPoint = point;
    SwipeGesture::state.isSwiping = false;
}

void ActivityManager::handleSwipeMove(const lv_point_t& point, const lv_point_t& startPoint) {
    if (!SwipeGesture::state.isSwiping && shouldStartSwipe(point, startPoint)) {
        SwipeGesture::state.isSwiping = true;
        
        Activity* currentActivity = activityStack.back();
        Activity* previousActivity = activityStack[activityStack.size() - 2];
        
        // 取消所有相关动画
        lv_anim_del(currentActivity->root, nullptr);
        lv_anim_del(previousActivity->root, nullptr);
        
        // 显示下层 activity
        lv_obj_clear_flag(previousActivity->root, LV_OBJ_FLAG_HIDDEN);
    }

    if (SwipeGesture::state.isSwiping) {
        lv_coord_t dx = point.x - startPoint.x;
        if (dx < 0) {
            dx = 0;
        }
        lv_obj_set_x(activityStack.back()->root, dx);
    }
}

void ActivityManager::handleSwipeEnd(const lv_point_t& startPoint) {
    SwipeGesture::state.isSwiping = false;
    
    Activity* currentActivity = activityStack.back();
    lv_coord_t currentX = lv_obj_get_x(currentActivity->root);
    lv_coord_t screenWidth = lv_obj_get_width(lv_screen_active());
    lv_coord_t threshold = screenWidth / 3;

    if (currentX > threshold) {
        // 滑动成功，关闭当前 activity
        activityStack.pop_back();
        disappear_Animation(currentActivity->root, AnimationCallbacks::destroyActivity, currentActivity);

        if (!activityStack.empty()) {
            activityStack.back()->onResume();
        }
    } else {
        // 滑动失败，弹回原位
        Activity* previousActivity = activityStack[activityStack.size() - 2];
        appear_Animation(currentActivity->root, AnimationCallbacks::hideObject, 
                        previousActivity->root, currentX);
    }
}

bool ActivityManager::shouldStartSwipe(const lv_point_t& current, const lv_point_t& start) const {
    lv_coord_t dx = current.x - start.x;
    lv_coord_t dy = std::abs(current.y - start.y);
    return dx > SwipeGesture::SWIPE_START_THRESHOLD && dx > dy;
}

void ActivityManager::appear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, 
                                     void* user_data, int startPosition) {
    if (!target) return;
    
    lv_anim_del(target, nullptr);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, target);
    lv_anim_set_time(&anim, ANIMATION_DURATION);
    lv_anim_set_values(&anim, startPosition, 0);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&anim, [](void* obj, int32_t v) {
        lv_obj_set_x(static_cast<lv_obj_t*>(obj), v);
    });

    if (ready_cb) {
        lv_anim_set_ready_cb(&anim, ready_cb);
        lv_anim_set_user_data(&anim, user_data);
    }
    
    lv_anim_start(&anim);
}

void ActivityManager::disappear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, 
                                        void* user_data) {
    if (!target) return;
    
    lv_anim_del(target, nullptr);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, target);
    lv_anim_set_time(&anim, ANIMATION_DURATION);
    lv_anim_set_values(&anim, lv_obj_get_x(target), lv_obj_get_width(lv_screen_active()));
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&anim, [](void* obj, int32_t v) {
        lv_obj_set_x(static_cast<lv_obj_t*>(obj), v);
    });

    if (ready_cb) {
        lv_anim_set_ready_cb(&anim, ready_cb);
        lv_anim_set_user_data(&anim, user_data);
    }

    lv_anim_start(&anim);
}
