#include "VideoPlayerActivity.h"

#include "framework/platform/DisplayManager.h"
#include "framework/ui/managers/FontManager.h"
#include "framework/systemui/StatusBarManager.h"
#include "lvgl.h"

// 构造函数
VideoPlayerActivity::VideoPlayerActivity(ActivityManager* manager, std::string path)
    : Activity(manager), videoPath_(std::move(path)) {}

// 析构函数
VideoPlayerActivity::~VideoPlayerActivity() {}

void VideoPlayerActivity::onCreate() {
    swipeToReturnEnabled = false;
    // 这里只需要一个黑色的背景让屏幕黑掉，这样稍后重设屏幕方向的时候用户不会察觉
    lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), 0);
}

// onResume会在入场动画播放完毕后执行
void VideoPlayerActivity::onResume() {
    // 将屏幕旋转到90度（横屏）
    DisplayManager::getInstance().setRotation(ScreenRotation::ROTATION_90);
    StatusBarManager::getInstance().hide();

    // 手动更新 root 容器的尺寸以匹配新的屏幕大小
    lv_obj_set_size(root, lv_obj_get_width(lv_screen_active()), lv_obj_get_height(lv_screen_active()));
    lv_obj_set_pos(root, 0, 0);

    // lv_obj_clean(root);
    // lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), 0); // 如果清掉了，需要重新设置背景色

    createUI();
}

void VideoPlayerActivity::onDestroy() {
    DisplayManager::getInstance().setRotation(ScreenRotation::ROTATION_0);
    StatusBarManager::getInstance().show();
}

void VideoPlayerActivity::createUI() {
    // 倍速播放提示器容器
    ui_Container2x = lv_obj_create(root);
    lv_obj_remove_style_all(ui_Container2x);
    lv_obj_set_width(ui_Container2x, 240);
    lv_obj_set_height(ui_Container2x, 60);
    lv_obj_set_x(ui_Container2x, 0);
    lv_obj_set_y(ui_Container2x, 40);
    lv_obj_set_align(ui_Container2x, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_Container2x, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));  /// Flags
    lv_obj_set_style_radius(ui_Container2x, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container2x, lv_color_hex(0x1D1D1D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container2x, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_Container2x, LV_OBJ_FLAG_HIDDEN);

    // 倍速提示器的三角形的容器
    ui_Container2xBox = lv_obj_create(ui_Container2x);
    lv_obj_remove_style_all(ui_Container2xBox);
    lv_obj_set_width(ui_Container2xBox, 100);
    lv_obj_set_height(ui_Container2xBox, 50);
    lv_obj_set_x(ui_Container2xBox, 5);
    lv_obj_set_y(ui_Container2xBox, 0);
    lv_obj_set_align(ui_Container2xBox, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_Container2xBox, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));  /// Flags

    ui_Triangle1 = lv_image_create(ui_Container2xBox);
    lv_image_set_src(ui_Triangle1, "L:/usr/share/myapp/assets/images/video/videoplayer_play.png");
    lv_obj_set_width(ui_Triangle1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Triangle1, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_x(ui_Triangle1, -22);
    lv_obj_set_y(ui_Triangle1, 0);
    lv_obj_set_align(ui_Triangle1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Triangle1, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_Triangle1, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_Triangle2 = lv_image_create(ui_Container2xBox);
    lv_image_set_src(ui_Triangle2, "L:/usr/share/myapp/assets/images/video/videoplayer_play.png");
    lv_obj_set_width(ui_Triangle2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Triangle2, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_Triangle2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Triangle2, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_Triangle2, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_Triangle3 = lv_image_create(ui_Container2xBox);
    lv_image_set_src(ui_Triangle3, "L:/usr/share/myapp/assets/images/video/videoplayer_play.png");
    lv_obj_set_width(ui_Triangle3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Triangle3, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_x(ui_Triangle3, 22);
    lv_obj_set_y(ui_Triangle3, 0);
    lv_obj_set_align(ui_Triangle3, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Triangle3, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_Triangle3, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_Label2x = lv_label_create(ui_Container2x);
    lv_obj_set_width(ui_Label2x, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2x, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_x(ui_Label2x, -18);
    lv_obj_set_y(ui_Label2x, 0);
    lv_obj_set_align(ui_Label2x, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_Label2x, "倍速播放中");
    lv_obj_set_style_text_color(ui_Label2x, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label2x, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label2x, FontManager::getFont(24), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ContainerHUD = lv_obj_create(root);
    lv_obj_remove_style_all(ui_ContainerHUD);
    lv_obj_set_width(ui_ContainerHUD, lv_pct(100));
    lv_obj_set_height(ui_ContainerHUD, lv_pct(100));
    lv_obj_set_align(ui_ContainerHUD, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_ContainerHUD, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));  /// Flags

    ui_BackButton = lv_image_create(ui_ContainerHUD);
    lv_image_set_src(ui_BackButton, "L:/usr/share/myapp/assets/images/video/videoplayer_back.png");
    lv_obj_set_width(ui_BackButton, 60);
    lv_obj_set_height(ui_BackButton, 60);
    lv_obj_set_x(ui_BackButton, -373);
    lv_obj_set_y(ui_BackButton, -198);
    lv_obj_set_align(ui_BackButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BackButton, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_BackButton, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_VideoName = lv_label_create(ui_ContainerHUD);
    lv_obj_set_width(ui_VideoName, 437);
    lv_obj_set_height(ui_VideoName, 24);
    lv_obj_set_x(ui_VideoName, 56);
    lv_obj_set_y(ui_VideoName, -201);
    lv_obj_set_align(ui_VideoName, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_VideoName, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_VideoName, "败犬女主太多了！02");
    lv_obj_set_style_text_color(ui_VideoName, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_VideoName, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_VideoName, FontManager::getFont(24), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SeekBar = lv_slider_create(ui_ContainerHUD);
    lv_slider_set_range(ui_SeekBar, 0, 1427);
    lv_slider_set_value(ui_SeekBar, 36, LV_ANIM_OFF);
    if (lv_slider_get_mode(ui_SeekBar) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_SeekBar, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_SeekBar, 754);
    lv_obj_set_height(ui_SeekBar, 10);
    lv_obj_set_x(ui_SeekBar, 0);
    lv_obj_set_y(ui_SeekBar, 154);
    lv_obj_set_align(ui_SeekBar, LV_ALIGN_CENTER);

    lv_obj_set_style_bg_color(ui_SeekBar, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_SeekBar, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Compensating for LVGL9.1 draw crash with bar/slider max value when top-padding is nonzero and right-padding is 0
    if (lv_obj_get_style_pad_top(ui_SeekBar, LV_PART_MAIN) > 0)
        lv_obj_set_style_pad_right(ui_SeekBar, lv_obj_get_style_pad_right(ui_SeekBar, LV_PART_MAIN) + 1, LV_PART_MAIN);
    ui_PlayButton = lv_image_create(ui_ContainerHUD);
    lv_image_set_src(ui_PlayButton, "L:/usr/share/myapp/assets/images/video/videoplayer_pause.png");
    lv_obj_set_width(ui_PlayButton, 70);
    lv_obj_set_height(ui_PlayButton, 50);
    lv_obj_set_x(ui_PlayButton, -362);
    lv_obj_set_y(ui_PlayButton, 199);
    lv_obj_set_align(ui_PlayButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_PlayButton, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_PlayButton, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_PlayTime = lv_label_create(ui_ContainerHUD);
    lv_obj_set_width(ui_PlayTime, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PlayTime, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_x(ui_PlayTime, 25);
    lv_obj_set_y(ui_PlayTime, 120);
    lv_obj_set_align(ui_PlayTime, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_PlayTime, "00:36/23:47");
    lv_obj_set_style_text_color(ui_PlayTime, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_PlayTime, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_PlayTime, FontManager::getFont(20), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MoreButton = lv_image_create(ui_ContainerHUD);
    lv_image_set_src(ui_MoreButton, "L:/usr/share/myapp/assets/images/video/videoplayer_more.png");
    lv_obj_set_width(ui_MoreButton, 60);
    lv_obj_set_height(ui_MoreButton, 60);
    lv_obj_set_x(ui_MoreButton, 367);
    lv_obj_set_y(ui_MoreButton, -198);
    lv_obj_set_align(ui_MoreButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_MoreButton, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_MoreButton, LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_CurrentTime = lv_label_create(ui_ContainerHUD);
    lv_obj_set_height(ui_CurrentTime, 24);
    lv_obj_set_width(ui_CurrentTime, LV_SIZE_CONTENT);  /// 437
    lv_obj_set_x(ui_CurrentTime, -66);
    lv_obj_set_y(ui_CurrentTime, -202);
    lv_obj_set_align(ui_CurrentTime, LV_ALIGN_RIGHT_MID);
    // lv_label_set_long_mode(ui_CurrentTime, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_CurrentTime, "00:00");
    lv_obj_set_style_text_color(ui_CurrentTime, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_CurrentTime, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_CurrentTime, FontManager::getFont(24), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_PlaySpeedButton = lv_obj_create(ui_ContainerHUD);
    lv_obj_remove_style_all(ui_PlaySpeedButton);
    lv_obj_set_width(ui_PlaySpeedButton, 100);
    lv_obj_set_height(ui_PlaySpeedButton, 60);
    lv_obj_set_x(ui_PlaySpeedButton, -4);
    lv_obj_set_y(ui_PlaySpeedButton, 207);
    lv_obj_set_align(ui_PlaySpeedButton, LV_ALIGN_RIGHT_MID);
    lv_obj_remove_flag(ui_PlaySpeedButton, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));  /// Flags

    ui_PlaySpeedText = lv_label_create(ui_PlaySpeedButton);
    lv_obj_set_width(ui_PlaySpeedText, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(ui_PlaySpeedText, LV_SIZE_CONTENT);  /// 50
    lv_obj_set_x(ui_PlaySpeedText, 0);
    lv_obj_set_y(ui_PlaySpeedText, -8);
    lv_obj_set_align(ui_PlaySpeedText, LV_ALIGN_CENTER);
    lv_label_set_text(ui_PlaySpeedText, "倍速");
    lv_obj_set_style_text_color(ui_PlaySpeedText, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_PlaySpeedText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_PlaySpeedText, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_PlaySpeedText, FontManager::getFont(24), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BatteryImage = lv_image_create(ui_ContainerHUD);
    lv_image_set_src(ui_BatteryImage, "L:/usr/share/myapp/assets/images/video/videoplayer_battery_5_37.png");
    lv_obj_set_width(ui_BatteryImage, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_BatteryImage, LV_SIZE_CONTENT);  /// 24
    lv_obj_set_x(ui_BatteryImage, 230);
    lv_obj_set_y(ui_BatteryImage, -199);
    lv_obj_set_align(ui_BatteryImage, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BatteryImage, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_obj_remove_flag(ui_BatteryImage, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
}