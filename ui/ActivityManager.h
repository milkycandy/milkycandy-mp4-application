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

    // 不是给 activity 用的，启用后全局生效
    void enableGlobalSwipe();

private:
    ActivityManager() = default;
    ~ActivityManager();

    void appear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data, int startPosition);
    void disappear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data);


private:
    std::vector<Activity*> activityStack;
};
