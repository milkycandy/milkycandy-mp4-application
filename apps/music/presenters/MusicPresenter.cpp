#include "MusicPresenter.h"
#include "apps/music/ui/MusicListActivity.h"

MusicPresenter::MusicPresenter(MusicListActivity* view) : view_(view) {}

void MusicPresenter::loadEntries(const std::string& path) {
    if (!view_) return;
    auto entries = repository_.getEntries(path, exts_);

    int folderCount = 0, fileCount = 0;
    for (auto& e : entries) (e.isDirectory ? folderCount : fileCount)++;

    view_->displayFileList(entries, folderCount, fileCount);
}

void MusicPresenter::onEntryClicked(const FileEntry& entry) {
    if (!view_) return;
    if (entry.isDirectory) view_->navigateToDirectory(entry.path);
    else                  view_->navigateToMusicPlayer(entry.path);
}
