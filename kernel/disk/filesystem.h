#ifndef DISK_FILESYSTEM_H
#define DISK_FILESYSTEM_H

#include "file.h"
#include "path.h"
#include <stdint.h>

// Virtual File System layer

namespace disk {
class Disk;
};

namespace filesystem {

class IFileSystem;

struct FileDescriptor {
    virtual ~FileDescriptor() {}

    uint32_t id;
    Path path;
    filesystem::IFileSystem *filesystem;
    disk::Disk *disk;
};

class IFileSystem {
public:
    virtual bool Resolve(disk::Disk *disk) = 0;
    virtual FileDescriptor *Open(disk::Disk *disk, const Path &path) = 0;
    virtual size_t Read(FileDescriptor *descriptor, size_t size, size_t count, char *buffer) = 0;
    virtual int Seek(FileDescriptor *descriptor, size_t offset, FileSeekMode whence) = 0;
    virtual int Stat(FileDescriptor *descriptor, FileStat *stat) = 0;
    virtual bool Close(FileDescriptor *descriptor) = 0;
};

void init_all();
IFileSystem *resolve(disk::Disk *disk);

}; // namespace filesystem

#endif // DISK_FILESYSTEM_H
