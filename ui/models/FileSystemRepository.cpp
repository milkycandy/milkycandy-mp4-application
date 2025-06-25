#include "FileSystemRepository.h"
#include <algorithm>
#include <dirent.h> // Linux/Unix 目录操作头文件
#include <sys/stat.h>

std::vector<FileEntry> FileSystemRepository::getEntries(const std::string& path) {
    std::vector<FileEntry> entries;
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        perror("opendir failed");
        return entries;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }

        std::string fullPath = path + "/" + name;
        struct stat st;
        if (stat(fullPath.c_str(), &st) == -1) {
            continue;
        }

        bool isDir = S_ISDIR(st.st_mode);
        bool isMp4 = !isDir && (name.length() > 4 && name.substr(name.length() - 4) == ".mp4");

        if (isDir || isMp4) {
            entries.push_back({name, fullPath, isDir});
        }
    }
    closedir(dir);

    // 文件夹在前，文件在后，内部按字母排序
    std::sort(entries.begin(), entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory;
        }
        return a.name < b.name;
    });

    return entries;
}