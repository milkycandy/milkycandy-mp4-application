#include "DisplayManager.h"
#include "src/drivers/display/drm/lv_linux_drm.h"
#include "src/drivers/evdev/lv_evdev.h"
#include <cstdio>

DisplayManager& DisplayManager::getInstance() {
    static DisplayManager instance;
    return instance;
}

void DisplayManager::init(ScreenRotation rotation, const char* indev_path) {
    disp_ = lv_drm_disp_create(static_cast<int>(rotation));
    if (!disp_) {
        fprintf(stderr, "Failed to create DRM display\n");
        return;
    }

    indev_ = lv_evdev_create(LV_INDEV_TYPE_POINTER, indev_path);
    lv_timer_t * t = lv_indev_get_read_timer(indev_);
    lv_timer_set_period(t, 16);

    configureInputDevice(rotation);
    currentRotation_ = rotation;
}

void DisplayManager::setRotation(ScreenRotation rotation) {

    printf("Rotating display to %d\n", static_cast<int>(rotation));
    lv_drm_disp_rotate(disp_, static_cast<int>(rotation));
    printf("Rotated display to %d\n", static_cast<int>(rotation));

    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        lv_display_set_resolution(disp_, 800, 480);
    } else {
        lv_display_set_resolution(disp_, 480, 800);
    }
    

    configureInputDevice(rotation);

    currentRotation_ = rotation;
    
    // lv_obj_invalidate(lv_screen_active());
}

void DisplayManager::configureInputDevice(ScreenRotation rotation) {
    if (!indev_) return;

    switch (rotation) {
        case ScreenRotation::ROTATION_0: // 竖屏 (0°)
            lv_evdev_set_swap_axes(indev_, false);
            // x: 0-480, y: 0-800
            lv_evdev_set_calibration(indev_, 0, 0, 480, 800);
            break;

        case ScreenRotation::ROTATION_90: // 横屏 (90°)
            lv_evdev_set_swap_axes(indev_, true);
            // x: 0-800, y: 0-480
            lv_evdev_set_calibration(indev_, 0, 480, 800, 0);
            break;
            
        case ScreenRotation::ROTATION_180: // 竖屏 (180°)
            lv_evdev_set_swap_axes(indev_, false);
            // x: 480-0, y: 800-0
            lv_evdev_set_calibration(indev_, 480, 800, 0, 0);
            break;

        case ScreenRotation::ROTATION_270: // 横屏 (270°)
            lv_evdev_set_swap_axes(indev_, true);
            // x: 800-0, y: 480-0
            lv_evdev_set_calibration(indev_, 800, 0, 0, 480);
            break;
    }
}

ScreenRotation DisplayManager::getCurrentRotation() const {
    return currentRotation_;
}

lv_display_t* DisplayManager::getDisplay() const {
    return disp_;
}

lv_indev_t* DisplayManager::getInputDevice() const {
    return indev_;
}