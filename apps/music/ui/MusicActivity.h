#pragma once
#include "framework/activity/Activity.h"
#include <string>

class MusicActivity : public Activity {
public:
    MusicActivity(ActivityManager* manager, std::string path);
    ~MusicActivity() override;

    void onCreate() override;
    void onDestroy() override;

private:
    static void back_button_event_cb(lv_event_t* e);
    std::string musicPath_;
};
