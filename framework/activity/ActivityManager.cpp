#include "ActivityManager.h"
#include "lvgl.h"
#include <cmath>
#include <algorithm>
#include "framework/platform/DisplayManager.h"

namespace {
    namespace Callbacks {
        // 动画完成回调：请求管理器彻底销毁 Activity
        void onExitAnimationComplete(lv_anim_t* a) {
            Activity* activity = static_cast<Activity*>(a->user_data);
            if (activity) {
                ActivityManager::getInstance().finalizeActivityDestruction(activity);
            }
        }

        // 动画完成回调：触发 Activity 的 onResume
        void onEnterAnimationComplete(lv_anim_t* a) {
            Activity* activity = static_cast<Activity*>(a->user_data);
            if (activity) {
                activity->onResume();
            }
        }

        // 动画完成回调：隐藏对象（用于底层 Activity 的暂时隐藏）
        void onHideObject(lv_anim_t* a) {
            lv_obj_t* obj = static_cast<lv_obj_t*>(a->user_data);
            if (obj) {
                lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }

    namespace Gesture {
        struct Context {
            lv_point_t startPoint;
            bool pressedLast = false;
            bool isSwiping = false;
        };
        
        static Context ctx;
        static constexpr lv_coord_t TRIGGER_THRESHOLD = 20;
        static constexpr int POLLING_MS = 16;
    }
}

ActivityManager& ActivityManager::getInstance() {
    static ActivityManager instance;
    return instance;
}

ActivityManager::~ActivityManager() {
    // unique_ptr 析构时会自动 delete Activity，进而调用 Activity 析构函数释放 LVGL 资源
    activityStack.clear();
    trashStack.clear();
}

void ActivityManager::startActivity(std::unique_ptr<Activity> newActivity) {
    if (!newActivity) return;

    Activity* currentTop = getCurrentActivity();
    if (currentTop) {
        currentTop->onPause();
    }

    // 获取原始指针用于后续的动画配置和创建操作
    Activity* rawPtr = newActivity.get();
    
    // 初始化 UI
    rawPtr->internal_create();

    // 关键操作：所有权转移入栈
    activityStack.push_back(std::move(newActivity));

    if (rawPtr->enableEnterAnimation) {
        playAppearAnimation(rawPtr->root, Callbacks::onEnterAnimationComplete, 
                          rawPtr, lv_obj_get_width(lv_screen_active()));
        
        // 为旧页面播放该隐动画（Dummy animation）以触发隐藏标志
        if (currentTop) {
            lv_anim_t hideAnim;
            lv_anim_init(&hideAnim);
            lv_anim_set_var(&hideAnim, currentTop->root);
            lv_anim_set_time(&hideAnim, ANIMATION_DURATION);
            lv_anim_set_values(&hideAnim, 0, 0);
            lv_anim_set_ready_cb(&hideAnim, Callbacks::onHideObject);
            lv_anim_set_user_data(&hideAnim, currentTop->root);
            lv_anim_start(&hideAnim);
        }
    } else {
        lv_obj_set_x(rawPtr->root, 0);
        rawPtr->onResume();
        if (currentTop) {
            lv_obj_add_flag(currentTop->root, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ActivityManager::finishCurrentActivity() {
    // 根 Activity 不允许被 finish
    if (activityStack.size() < 2) return;

    // 转移所有权：从运行栈移出
    std::unique_ptr<Activity> activityToFinish = std::move(activityStack.back());
    activityStack.pop_back();

    Activity* rawPtr = activityToFinish.get();
    rawPtr->onPause();
    
    // 移入垃圾回收栈，确保对象在动画期间存活
    trashStack.push_back(std::move(activityToFinish));

    // 恢复下层 Activity 可见性
    if (!activityStack.empty()) {
        Activity* newTop = activityStack.back().get();
        lv_obj_clear_flag(newTop->root, LV_OBJ_FLAG_HIDDEN);
        newTop->onResume();
    }
    
    // 播放退出动画，动画结束回调将触发真正的析构
    playDisappearAnimation(rawPtr->root, Callbacks::onExitAnimationComplete, rawPtr);
}

void ActivityManager::finalizeActivityDestruction(Activity* activityRawPtr) {
    // 在 trashStack 中查找匹配的 unique_ptr 并移除
    // 移除操作将触发 unique_ptr 的析构，从而安全释放 Activity 内存
    auto it = std::remove_if(trashStack.begin(), trashStack.end(),
        [activityRawPtr](const std::unique_ptr<Activity>& ptr) {
            return ptr.get() == activityRawPtr;
        });
    
    if (it != trashStack.end()) {
        trashStack.erase(it, trashStack.end());
    }
}

Activity* ActivityManager::getCurrentActivity() const {
    if (activityStack.empty()) {
        return nullptr;
    }
    return activityStack.back().get();
}

void ActivityManager::enableGlobalSwipe() {
    lv_timer_create([](lv_timer_t*) {
        auto& manager = ActivityManager::getInstance();
        if (manager.activityStack.size() <= 1) return;

        Activity* currentActivity = manager.activityStack.back().get();
        if (!currentActivity->swipeToReturnEnabled) return;

        lv_indev_t* indev = DisplayManager::getInstance().getInputDevice();
        if (!indev) return;

        lv_point_t currentPoint;
        lv_indev_get_point(indev, &currentPoint);
        lv_indev_state_t state = lv_indev_get_state(indev);
        
        using namespace Gesture;

        if (state == LV_INDEV_STATE_PRESSED) {
            if (!ctx.pressedLast) {
                manager.handleSwipeStart(currentPoint);
            }
            manager.handleSwipeMove(currentPoint, ctx.startPoint, indev);
        } else if (ctx.isSwiping) {
            manager.handleSwipeEnd(ctx.startPoint);
        }
        
        ctx.pressedLast = (state == LV_INDEV_STATE_PRESSED);
    }, Gesture::POLLING_MS, nullptr);
}

void ActivityManager::handleSwipeStart(const lv_point_t& point) {
    Gesture::ctx.startPoint = point;
    Gesture::ctx.isSwiping = false;
}

void ActivityManager::handleSwipeMove(const lv_point_t& point, const lv_point_t& startPoint, lv_indev_t* indev) {
    if (!Gesture::ctx.isSwiping && shouldStartSwipe(point, startPoint)) {
        Gesture::ctx.isSwiping = true;
        lv_indev_wait_release(indev);
        
        Activity* currentActivity = activityStack.back().get();
        Activity* previousActivity = activityStack[activityStack.size() - 2].get();
        
        // 停止正在进行的动画以防冲突
        lv_anim_del(currentActivity->root, nullptr);
        lv_anim_del(previousActivity->root, nullptr);
        
        // 预览下层 Activity
        lv_obj_clear_flag(previousActivity->root, LV_OBJ_FLAG_HIDDEN);
    }

    if (Gesture::ctx.isSwiping) {
        lv_coord_t dx = point.x - startPoint.x;
        if (dx < 0) dx = 0; // 禁止向左拖拽
        lv_obj_set_x(activityStack.back()->root, dx);
    }
}

void ActivityManager::handleSwipeEnd(const lv_point_t& startPoint) {
    Gesture::ctx.isSwiping = false;
    
    Activity* currentRaw = activityStack.back().get();
    lv_coord_t currentX = lv_obj_get_x(currentRaw->root);
    lv_coord_t screenWidth = lv_obj_get_width(lv_screen_active());
    lv_coord_t threshold = screenWidth / 3;

    if (currentX > threshold) {
        // 判定为返回：执行标准的 finish 流程
        // 手动模拟 finishCurrentActivity 的部分逻辑，因为已经发生了位移
        
        std::unique_ptr<Activity> ptr = std::move(activityStack.back());
        activityStack.pop_back();
        ptr->onPause();
        
        trashStack.push_back(std::move(ptr));

        if (!activityStack.empty()) {
            activityStack.back()->onResume();
        }
        
        playDisappearAnimation(currentRaw->root, Callbacks::onExitAnimationComplete, currentRaw);
        
    } else {
        // 判定为取消：回弹恢复
        Activity* previousRaw = activityStack[activityStack.size() - 2].get();
        playAppearAnimation(currentRaw->root, Callbacks::onHideObject, 
                          previousRaw->root, currentX);
    }
}

bool ActivityManager::shouldStartSwipe(const lv_point_t& current, const lv_point_t& start) const {
    lv_coord_t dx = current.x - start.x;
    lv_coord_t dy = std::abs(current.y - start.y);
    return dx > Gesture::TRIGGER_THRESHOLD && dx > dy;
}

void ActivityManager::playAppearAnimation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, 
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

void ActivityManager::playDisappearAnimation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, 
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