#pragma once
#include <string>

struct FileEntry {
    std::string name;
    std::string path;
    bool isDirectory;
};