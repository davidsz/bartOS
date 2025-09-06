#ifndef DISK_FAT16_H
#define DISK_FAT16_H

#include "filesystem.h"
#include <stdint.h>

// FAT16 File System

namespace filesystem {

class FAT16 : public IFileSystem {
public:
    // IFileSystem
    bool Resolve(disk::Disk *disk) override;
    FileDescriptor *Open(disk::Disk *disk, const Path &path) override;

    int CountItemsInDirectory(disk::Disk *disk, int start_sector_pos);
};

}; // namespace filesystem

#endif // DISK_FAT16_H
