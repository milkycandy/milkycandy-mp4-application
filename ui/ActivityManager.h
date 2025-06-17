#pragma once
#include <vector>
#include "Activity.h"

#define ANIMATION_DURATION     300

class ActivityManager {
public:
    static ActivityManager& getInstance();

    // 禁止拷贝和赋值
    ActivityManager(const ActivityManager&) = delete;
    ActivityManager& operator=(const ActivityManager&) = delete;

    void startActivity(Activity* newActivity);
    void finishCurrentActivity();
    Activity* getCurrentActivity() const;

    // 不是给 activity 用的，启用后全局生效
    void enableGlobalSwipe();

private:
    ActivityManager() = default;
    ~ActivityManager();

    void appear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data, int startPosition);
    void disappear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data);
    
    // 滑动手势辅助函数
    void handleSwipeStart(const lv_point_t& point);
    void handleSwipeMove(const lv_point_t& point, const lv_point_t& startPoint);
    void handleSwipeEnd(const lv_point_t& startPoint);
    bool shouldStartSwipe(const lv_point_t& current, const lv_point_t& start) const;

private:
    std::vector<Activity*> activityStack;
};
