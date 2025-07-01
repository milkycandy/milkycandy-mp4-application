#pragma once
#include "../Activity.h"
#include <memory>
#include <string>
#include <vector>

class VideoPlayerActivity : public Activity {
public:
    VideoPlayerActivity(ActivityManager* manager, std::string path);
    ~VideoPlayerActivity() override;

    void onCreate() override;
    void onDestroy() override;
    void onResume() override;

private:
    void createUI();

    std::string videoPath_;

    lv_obj_t* ui_Container2x;
    lv_obj_t* ui_Container2xBox;
    lv_obj_t* ui_Triangle1;
    lv_obj_t* ui_Triangle2;
    lv_obj_t* ui_Triangle3;
    lv_obj_t* ui_Label2x;
    lv_obj_t* ui_ContainerHUD;
    lv_obj_t* ui_BackButton;
    lv_obj_t* ui_VideoName;
    lv_obj_t* ui_SeekBar;
    lv_obj_t* ui_PlayButton;
    lv_obj_t* ui_PlayTime;
    lv_obj_t* ui_MoreButton;
    lv_obj_t* ui_CurrentTime;
    lv_obj_t* ui_PlaySpeedButton;
    lv_obj_t* ui_PlaySpeedText;
    lv_obj_t* ui_BatteryImage;

};