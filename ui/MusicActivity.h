#pragma once
#include "Activity.h"

class MusicActivity : public Activity {
public:
    using Activity::Activity;

    void onCreate() override;
    void onDestroy() override;

private:
    static void back_button_event_cb(lv_event_t* e);
};
