#ifndef STATUS_BAR_MANAGER_H
#define STATUS_BAR_MANAGER_H

#include "lvgl.h"
#include <string>

const int STATUS_BAR_HEIGHT = 48;

class StatusBarManager {
public:
    static StatusBarManager& getInstance();

    void init();

private:
    StatusBarManager() = default;
    StatusBarManager(const StatusBarManager&) = delete;
    StatusBarManager& operator=(const StatusBarManager&) = delete;

    // LVGL对象指针
    lv_obj_t* ui_StatusBar = nullptr;
    lv_obj_t* ui_ImageBluetooth = nullptr;
    lv_obj_t* ui_LabelTime = nullptr;
    lv_obj_t* ui_ImageBattery = nullptr;

};

#endif