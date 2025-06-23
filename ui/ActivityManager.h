#pragma once
#include <vector>
#include "Activity.h"

#define ANIMATION_DURATION     300

class ActivityManager {
public:
    /**
     * @brief 获取 ActivityManager 的唯一实例（单例模式）。
     * 
     * @return ActivityManager& 管理器实例引用。
     */
    static ActivityManager& getInstance();

    // 禁止拷贝和赋值
    ActivityManager(const ActivityManager&) = delete;
    ActivityManager& operator=(const ActivityManager&) = delete;

    /**
     * @brief 启动一个新的 Activity。
     * 
     * 当前顶部的 Activity（如果有）会进入onPause，新 Activity 会被压入栈顶并显示，
     * 
     * @param newActivity 新的 Activity 实例指针。
     */
    void startActivity(Activity* newActivity);

    /**
     * @brief 结束当前 Activity，并回到上一个 Activity。
     * 
     * 会播放退出动画并销毁当前 Activity，同时恢复上一个 Activity 的可见性和状态。
     * 如果栈中只有一个 Activity，则此函数无效。
     */
    void finishCurrentActivity();

    /**
     * @brief 获取当前栈顶（正在运行）的 Activity。
     * 
     * @return Activity* 当前 Activity 的指针，如果栈为空返回 nullptr。
     */
    Activity* getCurrentActivity() const;

    /**
     * @brief 启用全局滑动返回手势。
     * 
     * 启用后，从屏幕左侧向右滑动可以触发返回上一个 Activity 的操作，全局生效
     * 需要当前 Activity 没有禁用 `swipeToReturnEnabled`。
     */
    void enableGlobalSwipe();

private:
    ActivityManager() = default;
    ~ActivityManager();

    void appear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data, int startPosition);
    void disappear_Animation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data);
    
    // 滑动手势辅助函数
    void handleSwipeStart(const lv_point_t& point);
    void handleSwipeMove(const lv_point_t& point, const lv_point_t& startPoint, lv_indev_t* indev);
    void handleSwipeEnd(const lv_point_t& startPoint);
    bool shouldStartSwipe(const lv_point_t& current, const lv_point_t& start) const;

private:
    std::vector<Activity*> activityStack;
};
