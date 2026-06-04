/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "Platform/Include/ILocalFileSystem.h"
#include "Platform/Include/PathUtil.h"

#include <dirent.h>
#include <sys/stat.h>

namespace Platform {

class MacLocalFileSystem final : public ILocalFileSystem {
public:
    bool Exists(const std::string& path) const override
    {
        struct stat info {};
        return stat(Path::Normalize(path).c_str(), &info) == 0;
    }

    bool IsDirectory(const std::string& path) const override
    {
        struct stat info {};
        if (stat(Path::Normalize(path).c_str(), &info) != 0) {
            return false;
        }
        return S_ISDIR(info.st_mode);
    }

    bool IsRegularFile(const std::string& path) const override
    {
        struct stat info {};
        if (stat(Path::Normalize(path).c_str(), &info) != 0) {
            return false;
        }
        return S_ISREG(info.st_mode);
    }

    std::vector<std::string> ListDirectory(const std::string& path) const override
    {
        std::vector<std::string> entries;
        DIR* directory = opendir(Path::Normalize(path).c_str());
        if (directory == nullptr) {
            return entries;
        }

        while (dirent* entry = readdir(directory)) {
            if (entry->d_name[0] == '.' &&
                (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
                continue;
            }
            entries.emplace_back(entry->d_name);
        }

        closedir(directory);
        return entries;
    }
};

std::unique_ptr<ILocalFileSystem> CreateLocalFileSystem()
{
    return std::make_unique<MacLocalFileSystem>();
}

} // namespace Platform
