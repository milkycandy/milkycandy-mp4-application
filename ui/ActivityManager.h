#pragma once
#include <vector>
#include "Activity.h"

class ActivityManager {
public:
    static ActivityManager& getInstance();

    // 禁止拷贝和赋值
    ActivityManager(const ActivityManager&) = delete;
    ActivityManager& operator=(const ActivityManager&) = delete;

    void startActivity(Activity* newActivity);
    void finishCurrentActivity();

    // 在系统初始化后调用一次，启用全局右滑返回识别
    void enableGlobalSwipe();

private:
    ActivityManager() = default;
    ~ActivityManager();

    void addSwipeGesture(Activity* activity);
    void afterStartAnimation();

    // 静态的LVGL事件回调函数
    static void swipe_event_cb(lv_event_t* e);

    // --- 以下动画回调函数已被移除 ---
    // static void anim_finish_to_back_cb(lv_anim_t* anim);
    // static void anim_revert_to_current_cb(lv_anim_t* anim);
    // static void anim_start_new_ready_cb(lv_anim_t* anim);

private:
    std::vector<Activity*> activityStack;
};
