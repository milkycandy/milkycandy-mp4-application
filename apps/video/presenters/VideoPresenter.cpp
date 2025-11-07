#include "VideoPresenter.h"
#include "apps/video/ui/VideoListActivity.h"

VideoPresenter::VideoPresenter(VideoListActivity* view) : view_(view) {}

void VideoPresenter::loadEntries(const std::string& path) {
    if (!view_) return;

    // 从 Model 获取数据
    auto entries = repository_.getEntries(path, exts_);

    // 计算文件夹和文件数量
    int folderCount = 0;
    int fileCount = 0;
    for (const auto& entry : entries) {
        if (entry.isDirectory) {
            folderCount++;
        } else {
            fileCount++;
        }
    }

    // 调用 View 的方法去更新 UI
    view_->displayFileList(entries, folderCount, fileCount);
}

void VideoPresenter::onEntryClicked(const FileEntry& entry) {
    if (!view_) return;

    if (entry.isDirectory) {
        view_->navigateToDirectory(entry.path);
    } else {
        view_->navigateToVideoPlayer(entry.path);
    }
}