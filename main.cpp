#include <lvgl.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "ui/ActivityManager.h"
#include "ui/activities/LauncherActivity.h"
#include "ui/activities/MusicActivity.h"
#include "ui/managers/FontManager.h"
#include "ui/managers/ScreenManager.h"
#include "ui/managers/StatusBarManager.h"
#include "ui/managers/StyleManager.h"

// 精准的 1ms LVGL tick 定时器
void *tickThread(void *data) {
    (void)data;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1 * 1000 * 1000;  // 1ms

    while (1) {
        lv_tick_inc(1);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
    }

    return NULL;
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("[LVGL] GoGoGo!\n");

    lv_init();

    // 启动精确 tick 定时线程
    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tickThread, NULL);

    ScreenManager::getInstance().init(ScreenRotation::ROTATION_0, "/dev/input/event0");

    StyleManager::init();

    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

    StatusBarManager::getInstance().init();

    ActivityManager &manager = ActivityManager::getInstance();

    manager.startActivity(new LauncherActivity(&manager));
    manager.enableGlobalSwipe();

    // 主循环
    while (1) {
        lv_timer_handler();

        usleep(200);  // 主线程休眠 0.2ms
    }

    return 0;
}
