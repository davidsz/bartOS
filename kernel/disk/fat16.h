#ifndef DISK_FAT16_H
#define DISK_FAT16_H

#include "filesystem.h"
#include "unique.h"
#include <stdint.h>

// FAT16 File System

namespace disk {
class Stream;
}

namespace filesystem {

struct FAT_Directory_Item;
struct FAT_Directory;
struct FAT_Disk_Data;
struct FAT_Item;

class FAT16 : public IFileSystem {
public:
    // IFileSystem
    bool Resolve(disk::Disk *disk) override;
    FileDescriptor *Open(disk::Disk *disk, const Path &path) override;
    size_t Read(FileDescriptor *descriptor, size_t size, size_t count, char *buffer) override;

    int CountItemsInDirectory(disk::Disk *disk, int start_sector_pos);
    Unique<FAT_Item> FindItemByNameInDirectory(disk::Disk *disk, const FAT_Directory &directory, const String &name);
    FAT_Directory *LoadDirectory(disk::Disk *disk, const FAT_Directory_Item *directory_item);
    uint32_t GetFirstCluster(const FAT_Directory_Item *directory_item);
    int ClusterToSector(FAT_Disk_Data *disk_data, uint32_t cluster);
    void ReadInternal(disk::Disk *disk, uint32_t starting_cluster, int offset, int total, void *out);
    void ReadInternalFromStream(disk::Disk *disk, disk::Stream *stream, uint32_t cluster, int offset, int total, void *out);
    int GetClusterForOffset(disk::Disk *disk, uint32_t starting_cluster, int offset);
    uint16_t GetFATEntry(disk::Disk *disk, int cluster);
};

}; // namespace filesystem

#endif // DISK_FAT16_H
