#pragma once

#include "lvgl.h"

enum class ScreenRotation {
    ROTATION_0 = 0,
    ROTATION_90 = 90,
    ROTATION_180 = 180,
    ROTATION_270 = 270
};

class ScreenManager {
public:
    // 获取单例实例
    static ScreenManager& getInstance();

    // 禁止拷贝和赋值
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

    /**
     * @brief 初始化屏幕和输入设备
     * @param rotation 屏幕旋转角度
     * @param indev_path 输入设备的路径 (例如 "/dev/input/event0")
     */
    void init(ScreenRotation rotation, const char* indev_path);

    /**
     * @brief 设置屏幕旋转
     * @param rotation 新的旋转角度
     * @note 此操作会销毁并重新创建显示设备，可能会导致短暂的黑屏。
     * 之后会自动更新触摸输入设备的映射关系。
     */
    void setRotation(ScreenRotation rotation);

    /**
     * @brief 获取当前屏幕旋转角度
     * @return 当前的旋转角度
     */
    ScreenRotation getCurrentRotation() const;

    /**
     * @brief 获取当前的显示设备指针
     * @return lv_display_t*
     */
    lv_display_t* getDisplay() const;

    /**
     * @brief 获取当前的输入设备指针
     * @return lv_indev_t*
     */
    lv_indev_t* getInputDevice() const;

private:
    ScreenManager() = default;
    ~ScreenManager() = default;

    void configureInputDevice(ScreenRotation rotation);

    lv_display_t* disp_ = nullptr;
    lv_indev_t* indev_ = nullptr;
    ScreenRotation currentRotation_;
};