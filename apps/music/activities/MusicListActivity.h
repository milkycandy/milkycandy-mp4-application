#pragma once
#include "framework/activity/Activity.h"
#include <memory>
#include <vector>
#include <string>
#include "libs/models/FileEntry.h"

class MusicPresenter;

class MusicListActivity : public Activity {
public:
    MusicListActivity(ActivityManager* manager, std::string path);
    ~MusicListActivity() override;

protected:
    void onCreate() override;
    void onDestroy() override;

public: // 被 Presenter 调用
    void displayFileList(const std::vector<FileEntry>& entries, int folderCount, int fileCount);
    void navigateToDirectory(const std::string& path);
    void navigateToMusicPlayer(const std::string& path);

private:
    static void list_item_event_cb(lv_event_t* e);
    void createListItem(lv_obj_t* parent, FileEntry* entry, int index);

private:
    std::unique_ptr<MusicPresenter> presenter_;
    std::string currentPath_;
    lv_obj_t* listContainer_ = nullptr;
    lv_obj_t* footerLabel_ = nullptr;
    std::vector<FileEntry*> listEntryData_;
};