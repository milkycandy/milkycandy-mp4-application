#pragma once
#include "lvgl.h"

// 前向声明，避免与 ActivityManager.h 循环依赖
class ActivityManager;

class Activity {
public:
    // 构造函数，需要一个指向 ActivityManager 的指针
    explicit Activity(ActivityManager* manager);

    // 虚析构函数，是良好C++实践的一部分，确保子类能被正确销毁
    virtual ~Activity() = default;

    // 禁止拷贝和赋值，因为每个Activity实例都是唯一的
    Activity(const Activity&) = delete;
    Activity& operator=(const Activity&) = delete;

    // --- 生命周期方法 (由子类实现) ---
    virtual void onCreate() = 0;
    virtual void onResume();
    virtual void onPause();
    virtual void onDestroy();

    // --- 内部方法 (由 ActivityManager 调用) ---
    void create();
    void destroy();

public:
    lv_obj_t* root = nullptr;                 // 每个Activity的根LVGL对象
    bool swipeToReturnEnabled = true;      // 是否允许右滑返回
    bool enableEnterAnimation = true;

protected:
    ActivityManager* manager;       // 指向管理器的指针
};
