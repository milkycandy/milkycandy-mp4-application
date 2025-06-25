#pragma once
#include "../models/FileEntry.h"
#include "../models/FileSystemRepository.h"
#include <string>

// 前向声明，避免头文件循环引用
class VideoListActivity;

class VideoPresenter {
public:
    VideoPresenter(VideoListActivity* view);

    void loadEntries(const std::string& path);
    void onEntryClicked(const FileEntry& entry);

private:
    VideoListActivity* view_; // 指向 View 的指针
    FileSystemRepository repository_;
};