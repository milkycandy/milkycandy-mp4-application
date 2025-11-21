#pragma once
#include <vector>
#include <memory>
#include "Activity.h"

#define ANIMATION_DURATION     300

/**
 * @class ActivityManager
 * @brief 全局页面栈管理器（单例模式）。
 *
 * 负责应用程序内 Activity 的导航、生命周期分发、转场动画管理以及全局手势处理。
 * 该类拥有 Activity 实例的绝对所有权。
 */
class ActivityManager {
public:
/**
     * @brief 获取管理器单例。
     * 
     * @return ActivityManager& 全局唯一的管理器引用。
     */
    static ActivityManager& getInstance();

    // 禁止拷贝和赋值
    ActivityManager(const ActivityManager&) = delete;
    ActivityManager& operator=(const ActivityManager&) = delete;

    /**
     * @brief 压入并启动一个新的 Activity。
     * * @param newActivity 独占指针，指向待启动的 Activity。
     */
    void startActivity(std::unique_ptr<Activity> newActivity);

    /**
     * @brief 结束当前 Activity 并返回上一级。
     * * 将当前 Activity 移出运行栈，播放退出动画，并在动画结束后自动销毁实例。
     * 如果栈中仅剩一个 Activity，此操作将被忽略。
     */
    void finishCurrentActivity();

    /**
     * @brief 获取当前栈顶 Activity。
     * @return Activity* 原始指针（观察者视角），若栈为空则返回 nullptr。
     */
    Activity* getCurrentActivity() const;

    /**
     * @brief 启用全局滑动返回手势。
     * 
     * 启用后，从屏幕左侧向右滑动可以返回上一个 Activity ，全局生效
     * 需要当前 Activity 没有禁用 `swipeToReturnEnabled`。
     */
    void enableGlobalSwipe();

    /**
     * @brief 从垃圾回收栈中移除并销毁 Activity。
     * * @note 仅供动画回调系统内部使用。
     * @param activityRawPtr 待销毁对象的原始指针（用于查找）。
     */
    void finalizeActivityDestruction(Activity* activityRawPtr);

private:
    ActivityManager() = default;
    ~ActivityManager();

    void playAppearAnimation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data, int startPosition);
    void playDisappearAnimation(lv_obj_t* target, lv_anim_ready_cb_t ready_cb, void* user_data);    

    // 滑动手势辅助函数
    void handleSwipeStart(const lv_point_t& point);
    void handleSwipeMove(const lv_point_t& point, const lv_point_t& startPoint, lv_indev_t* indev);
    void handleSwipeEnd(const lv_point_t& startPoint);
    bool shouldStartSwipe(const lv_point_t& current, const lv_point_t& start) const;

private:
    /**
     * @brief 活跃 Activity 栈。
     * 栈顶元素为当前显示的 Activity。
     */
    std::vector<std::unique_ptr<Activity>> activityStack;

    /**
     * @brief 待销毁 Activity 缓冲区（垃圾回收栈）。
     * * 存放已经逻辑退出（finish）但正在播放退出动画的 Activity。
     * 智能指针在此处保持对象的存活，直到动画结束回调触发清理。
     */
    std::vector<std::unique_ptr<Activity>> trashStack;
};
