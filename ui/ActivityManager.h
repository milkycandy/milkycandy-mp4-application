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

    // 不是给 activity 用的，启用后全局生效
    void enableGlobalSwipe();

private:
    ActivityManager() = default;
    ~ActivityManager();

    void addSwipeGesture(Activity* activity);
    void afterStartAnimation();


private:
    std::vector<Activity*> activityStack;
};
