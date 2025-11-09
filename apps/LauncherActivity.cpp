#include "LauncherActivity.h"

#include "framework/activity/ActivityManager.h"
#include "music/ui/MusicActivity.h"
#include "video/ui/VideoListActivity.h"
#include "music/ui/MusicListActivity.h"
#include "lvgl.h"
#include "framework/ui/managers/StyleManager.h"
#include "framework/ui/widgets/Dialog.h"
#include "framework/ui/widgets/Toast.h"

void LauncherActivity::onCreate() {
    enableEnterAnimation = false;  // 关闭进入动画

    lv_obj_set_style_bg_image_src(lv_screen_active(), "L:/usr/share/myapp/assets/images/launcher/wallpaper.png", 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), 255, 0);

    lv_obj_set_style_bg_opa(root, 0, 0);

    // lv_obj_set_style_bg_image_src(root, &wallpaper, 0);
    // lv_obj_set_style_bg_image_recolor(root, lv_color_hex(0xA440FF), 0);
    // lv_obj_set_style_bg_image_recolor_opa(root, 66, 0);

    lv_obj_t* ui_HomePage = lv_obj_create(root);
    lv_obj_remove_style_all(ui_HomePage);
    
    lv_obj_add_style(ui_HomePage, StyleManager::getHomepageStyle(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_HomePage, lv_pct(100));
    lv_obj_set_height(ui_HomePage, lv_pct(100));
    lv_obj_set_align(ui_HomePage, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(ui_HomePage, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(ui_HomePage, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_HomePage, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scroll_dir(ui_HomePage, LV_DIR_VER);

    auto createTile = [](lv_obj_t* parent, const void* imgSrc, const char* labelText, lv_event_cb_t event_cb) {
        lv_obj_t* container = lv_obj_create(parent);
        lv_obj_remove_style_all(container);
        lv_obj_add_style(container, StyleManager::getTileContainerStyle(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_align(container, LV_ALIGN_CENTER);
        lv_obj_remove_flag(container, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));

        lv_obj_t* imgBtn = lv_imagebutton_create(container);
        lv_imagebutton_set_src(imgBtn, LV_IMAGEBUTTON_STATE_RELEASED, nullptr, imgSrc, nullptr);
        lv_imagebutton_set_src(imgBtn, LV_IMAGEBUTTON_STATE_PRESSED, nullptr, imgSrc, nullptr);
        lv_obj_set_width(imgBtn, 192);
        lv_obj_set_height(imgBtn, 192);
        lv_obj_set_align(imgBtn, LV_ALIGN_TOP_MID);
        lv_obj_add_style(imgBtn, StyleManager::getPressedImageButtonStyle(), LV_PART_MAIN | LV_STATE_PRESSED);

        if (event_cb) {
            lv_obj_add_event_cb(imgBtn, event_cb, LV_EVENT_RELEASED, nullptr);
        }

        lv_obj_t* label = lv_label_create(container);
        lv_obj_set_width(label, LV_SIZE_CONTENT);
        lv_obj_set_height(label, LV_SIZE_CONTENT);
        lv_obj_set_x(label, 0);
        lv_obj_set_y(label, -11);
        lv_obj_set_align(label, LV_ALIGN_BOTTOM_MID);
        lv_label_set_text(label, labelText);
        lv_obj_add_style(label, StyleManager::getTileLabelStyle(), LV_PART_MAIN | LV_STATE_DEFAULT);

        return container;
    };

    // 创建具体条目
    lv_obj_t* ui_ContainerMusic = createTile(ui_HomePage, "L:/usr/share/myapp/assets/images/launcher/music.png", "音乐", launchMusicListActivity);
    lv_obj_t* ui_ContainerVideo = createTile(ui_HomePage, "L:/usr/share/myapp/assets/images/launcher/video.png", "视频", launchVideoListActivity);
    lv_obj_t* ui_ContainerSettings = createTile(ui_HomePage, "L:/usr/share/myapp/assets/images/launcher/settings.png", "设置", launchSettingsActivity);

    // 占位容器（无图、无标签）
    lv_obj_t* ui_ContainerPlaceHolder = lv_obj_create(ui_HomePage);
    lv_obj_remove_style_all(ui_ContainerPlaceHolder);
    lv_obj_add_style(ui_ContainerPlaceHolder, StyleManager::getTileContainerStyle(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_ContainerPlaceHolder, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_ContainerPlaceHolder, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
}

void LauncherActivity::onResume() {
    LV_LOG_USER("LauncherActivity onResume");
}

void LauncherActivity::onPause() {
    LV_LOG_USER("LauncherActivity onPause");
}

void LauncherActivity::onDestroy() {
    // TODO: 在设备关机时销毁所有活动
}

void LauncherActivity::launchMusicListActivity(lv_event_t* e) {
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);  // 强制转换
    lv_indev_t* indev = lv_indev_active();
    
    // 检查释放时手指/光标是否还在按钮区域内
    if(indev) {
        lv_point_t point;
        lv_indev_get_point(indev, &point);
        
        // 判断释放点是否在对象范围内
        if(lv_obj_hit_test(obj, &point)) {
            ActivityManager& manager = ActivityManager::getInstance();
            manager.startActivity(new MusicListActivity(&manager, "/root/music"));
        }
        // 如果不在范围内,则不触发,相当于"放弃点击"
    }
    // Toast::show("你好，我是Toast！");
    // Dialog::showDualButton(
    //     "你好",
    //     "我是一个双按钮Dialog! 你想见见单按钮Dialog吗？天地玄黄，宇宙洪荒。日月盈昃，辰宿列张。The quick brown fox
    //     jumps over the lazy dog. 1234567890。こんにちは、世界！今日はいい天気ですね。富士山が見える。",
    //     []() {
    //         Toast::show("来了！");
    //         Dialog::showSingleButton(
    //             "Hi",
    //             "我是单按钮Dialog，向你问好！",
    //              []() {
    //                 Toast::show("下次见！");
    //              }
    //         );
    //     },
    //     []() {
    //         Toast::show("那拜拜！");
    //     },
    //     "好捏",
    //     "不要"
    // );
}

void LauncherActivity::launchVideoListActivity(lv_event_t* e) {
    ActivityManager& manager = ActivityManager::getInstance();
    // 启动 VideoListActivity 并传入指定的初始路径
    manager.startActivity(new VideoListActivity(&manager, "/root/video")); 
}

void LauncherActivity::launchSettingsActivity(lv_event_t* e) {
    Toast::show("没有这种东西！");

}
