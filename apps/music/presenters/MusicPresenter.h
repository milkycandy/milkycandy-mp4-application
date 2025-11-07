#pragma once
#include <string>
#include <unordered_set>
#include "libs/models/FileSystemRepository.h"
#include "libs/models/FileEntry.h"

class MusicListActivity; // 前置声明

class MusicPresenter {
public:
    explicit MusicPresenter(MusicListActivity* view);
    void loadEntries(const std::string& path);
    void onEntryClicked(const FileEntry& entry);
private:
    MusicListActivity* view_ = nullptr;
    FileSystemRepository repository_;
    const std::unordered_set<std::string> exts_{".mp3",".flac",".wav",".aac",".ogg",".m4a"};
};
