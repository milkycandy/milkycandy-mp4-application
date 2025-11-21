#pragma once
#include "framework/activity/Activity.h"
#include <string>
#include <memory>
#include "services/AudioPlayer/GStreamerAudioPlayer.h"
#include "services/AudioPlayer/AudioService.h"
#include "../views/MusicPlayerView.h"

class MusicActivity : public Activity {
public:
    MusicActivity(ActivityManager* manager, std::string path);
    ~MusicActivity() override;

    void onCreate() override;
    void onResume() override;
    void onPause() override;
    void onDestroy() override;

private:
    static void back_button_event_cb(lv_event_t* e);
    std::string musicPath_;
    

    std::unique_ptr<MusicPlayerView> view;
    int progressListenerId_ = 0;
    int stateListenerId_ = 0;
    int metadataListenerId_ = 0;
    int32_t lastDurationMs_ = -1;
};
