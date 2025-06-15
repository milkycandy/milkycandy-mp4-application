#include <lvgl.h>
#include "src/drivers/display/drm/lv_linux_drm.h"
#include "src/drivers/evdev/lv_evdev.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "ui/ActivityManager.h"
#include "ui/HomeActivity.h"
#include "ui/MusicActivity.h"
// #include "ui/ui.h"

// 精准的 1ms LVGL tick 定时器
void *tick_thread(void *data)
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

static void print_touch_position(lv_timer_t * timer) {
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while (indev) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            lv_indev_state_t state = lv_indev_get_state(indev);
            if (state == LV_INDEV_STATE_PRESSED) {
                printf("[TOUCH] Pressed at (%d, %d)\n", p.x, p.y);
            }
        }
        indev = lv_indev_get_next(indev);
    }
}


int main(void)
{
    printf("[LVGL] GoGoGo!\n");

    lv_init();

    // 启动精确 tick 定时线程
    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tick_thread, NULL);

    // 初始化 DRM 显示
    lv_display_t * disp = lv_drm_disp_create(0);
    if (!disp) {
        fprintf(stderr, "Failed to create DRM display\n");
        return 1;
    }

    // 初始化输入设备
    lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");

    // 启动demo
    // ui_init();
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

    // ----------------------------
    lv_timer_create(print_touch_position, 500, NULL);

    ActivityManager &manager = ActivityManager::getInstance();
    manager.startActivity(new HomeActivity(&manager));

    // 主循环
    while (1) {
        lv_timer_handler();
        usleep(1000); // 主线程休眠 1ms
    }

    return 0;
}
