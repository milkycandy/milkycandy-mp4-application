#include <lvgl.h>
// #include "src/drivers/display/drm/lv_linux_drm.h"
// #include "src/drivers/evdev/lv_evdev.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "ui/managers/ScreenManager.h"
#include "ui/ActivityManager.h"
#include "ui/HomeActivity.h"
#include "ui/MusicActivity.h"
#include "ui/managers/FontManager.h"
#include "ui/managers/StyleManager.h"
// #include "ui/ui.h"

// 精准的 1ms LVGL tick 定时器
void *tickThread(void *data)
{
    (void)data;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1 * 1000 * 1000; // 1ms

    while (1) {
        lv_tick_inc(1);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
    }

    return NULL;
}

int main(void)
{
    printf("[LVGL] GoGoGo!\n");

    lv_init();

    // 启动精确 tick 定时线程
    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tickThread, NULL);

    ScreenManager::getInstance().init(ScreenRotation::ROTATION_0, "/dev/input/event0");

    StyleManager::init();

    // 启动demo
    // ui_init();
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);


    ActivityManager &manager = ActivityManager::getInstance();
    
    manager.startActivity(new HomeActivity(&manager));
    manager.enableGlobalSwipe();


    // lv_timer_create([](lv_timer_t* t){
    //      static int rotation_angle_index = -1;
    //      ScreenRotation rotations[] = {
    //          ScreenRotation::ROTATION_0,
    //          ScreenRotation::ROTATION_90,
    //          ScreenRotation::ROTATION_270,
    //          ScreenRotation::ROTATION_180
    //      };
    //      rotation_angle_index = (rotation_angle_index + 1) % 4;
    //      printf("rotation_angle_index: %d\n", rotation_angle_index);
    //      ScreenManager::getInstance().setRotation(rotations[rotation_angle_index]);
    //      printf("Screen rotation changed!\n");
    // }, 2000, nullptr);

    // 主循环
    while (1) {
        lv_timer_handler();
        
        usleep(1000); // 主线程休眠 1ms
    }

    return 0;
}
