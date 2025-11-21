#include <lvgl.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "framework/activity/ActivityManager.h"
#include "apps/LauncherActivity.h"
#include "framework/ui/managers/FontManager.h"
#include "framework/platform/DisplayManager.h"
#include "framework/systemui/StatusBarManager.h"
#include "framework/ui/managers/StyleManager.h"
#include "services/AudioPlayer/AudioService.h"

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

    // 启动 tick 定时线程
    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tickThread, NULL);

    DisplayManager::getInstance().init(ScreenRotation::ROTATION_0, "/dev/input/event0");

    FontManager::init();
    StyleManager::init();

    AudioService::getInstance().init();

    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

    StatusBarManager::getInstance().init();

    ActivityManager &manager = ActivityManager::getInstance();

    manager.startActivity(
        std::make_unique<LauncherActivity>(&manager)
    );
    manager.enableGlobalSwipe();

    // 主循环
    while (1) {
        lv_timer_handler();

        usleep(200);
        // uint32_t sleep_ms = lv_timer_handler();

        // usleep(sleep_ms * 1000);
    }

    return 0;
}
