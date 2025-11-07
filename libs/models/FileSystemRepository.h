#pragma once
#include "FileEntry.h"
#include <string>
#include <vector>
#include <unordered_set>

class FileSystemRepository {
public:
    std::vector<FileEntry> getEntries(const std::string& path, const std::unordered_set<std::string>& allowExts = {});
};