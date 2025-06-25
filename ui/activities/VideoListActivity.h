#pragma once
#include "../Activity.h"
#include "../models/FileEntry.h"
#include "../presenters/VideoPresenter.h"
#include <memory>
#include <string>
#include <vector>

class VideoListActivity : public Activity {
public:
    // 构造函数接收路径
    VideoListActivity(ActivityManager* manager, std::string path);
    ~VideoListActivity() override;

    // 给 Presenter 调用
    void displayFileList(const std::vector<FileEntry>& entries, int folderCount, int fileCount);
    void navigateToDirectory(const std::string& path);
    void navigateToVideoPlayer(const std::string& path);

protected:
    void onCreate() override;
    void onDestroy() override;

private:
    void createListItem(lv_obj_t* parent, const FileEntry& entry, int index);
    
    static void list_item_event_cb(lv_event_t* e);

    std::string currentPath_;
    std::unique_ptr<VideoPresenter> presenter_;
    std::vector<FileEntry*> listEntryData_;

    // UI 控件
    lv_obj_t* listContainer_;
    lv_obj_t* footerLabel_;
};