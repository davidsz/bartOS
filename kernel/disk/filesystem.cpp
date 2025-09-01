#include "filesystem.h"
#include "fat16.h"
#include "list.h"

namespace filesystem {

static List<IFileSystem *> s_filesystems;

void init_all()
{
    s_filesystems.push_back(new filesystem::FAT16);
}

IFileSystem *resolve(disk::Disk *disk)
{
    for (auto it = s_filesystems.begin(); it != s_filesystems.end(); ++it) {
        if ((*it)->Resolve(disk))
            return *it;
    }
    return nullptr;
}

}; // namespace filesystem
