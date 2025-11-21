#pragma once
#include "lvgl.h"
#include <memory>

// 前向声明，避免与 ActivityManager.h 循环依赖
class ActivityManager;

/**
 * @class Activity
 * @brief 应用程序页面的抽象基类。
 *
 * Activity 代表一个独立的屏幕或交互模块。它封装了 LVGL 的视图，用于替代 Screen。
 * * @note 这是一个抽象类，子类必须实现 onCreate() 方法。
 * @warning Activity 实例由 ActivityManager 独占管理，禁止手动 delete。
 */
class Activity {
public:
    /**
     * @brief 构造 Activity 实例。
     * @param manager 指向 ActivityManager 单例的指针，用于页面导航。
     */
    explicit Activity(ActivityManager* manager);

    /**
     * @brief 虚析构函数。
     * * 负责清理 Activity 占用的所有资源，包括销毁根 LVGL 对象。
     * 当 Activity 从管理器的栈中移除时，此函数会被自动调用。
     */
    virtual ~Activity();

    // 禁止拷贝和赋值，因为每个 Activity 实例都是唯一的
    Activity(const Activity&) = delete;
    Activity& operator=(const Activity&) = delete;

    /**
     * @brief 页面创建回调。
     * * 子类必须实现此方法以构建 UI 布局。
     * 在此方法调用时，root 对象已被创建并初始化为全屏容器。
     */
    virtual void onCreate() = 0;

    /**
     * @brief 页面恢复可见回调。
     * * 当 Activity 入栈成为栈顶，或上层 Activity 退出导致此 Activity 重新显示时调用。
     * 适合恢复动画、或刷新数据。
     */
    virtual void onResume();

    /**
     * @brief 页面暂停回调。
     * * 当启动新 Activity 覆盖当前页面，或当前页面即将退出时调用。
     * 适合暂停动画、或保存临时状态。
     */
    virtual void onPause();

    /**
     * @brief 页面销毁回调。
     * * 在析构函数执行前调用。子类可在此释放非 LVGL 的自定义资源。
     */
    virtual void onDestroy();

public:
    /**
     * @brief 内部生命周期方法，初始化基础设施。
     * @note 由 ActivityManager 调用，外部禁止调用。
     */
    void internal_create();

public:
    lv_obj_t* root = nullptr;           ///< Activity 的根 LVGL 容器对象
    bool swipeToReturnEnabled = true;   ///< 标志位：是否允许通过手势返回上一级
    bool enableEnterAnimation = true;   ///< 标志位：是否启用入场动画

protected:
    ActivityManager* manager;       // 指向管理器的指针
};
