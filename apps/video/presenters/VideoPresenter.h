#pragma once
#include "libs/models/FileEntry.h"
#include "libs/models/FileSystemRepository.h"
#include <string>
#include <unordered_set>

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
    const std::unordered_set<std::string> exts_{".mp4"};
};