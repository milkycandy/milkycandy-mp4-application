#pragma once
#include "../models/FileEntry.h"
#include "../models/FileSystemRepository.h"
#include <string>

// 前向声明，避免头文件循环引用
class VideoActivity;

class VideoPresenter {
public:
    VideoPresenter(VideoActivity* view);

    void loadEntries(const std::string& path);
    void onEntryClicked(const FileEntry& entry);

private:
    VideoActivity* view_; // 指向 View 的指针
    FileSystemRepository repository_;
};