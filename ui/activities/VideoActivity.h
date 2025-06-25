#pragma once
#include "../Activity.h"

class VideoActivity : public Activity {
public:
    using Activity::Activity;

    void onCreate() override;
    void onDestroy() override;

private:
};
