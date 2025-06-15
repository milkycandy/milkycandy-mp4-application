#pragma once
#include "Activity.h"

class HomeActivity : public Activity {
public:
    using Activity::Activity; // 继承基类构造函数

    void onCreate() override;
    void onResume() override;
    void onPause() override;

private:
    static void go_to_music_event_cb(lv_event_t* e);
};
