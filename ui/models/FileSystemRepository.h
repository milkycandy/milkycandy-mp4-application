#pragma once
#include "FileEntry.h"
#include <string>
#include <vector>

class FileSystemRepository {
public:
    std::vector<FileEntry> getEntries(const std::string& path);
};