#ifndef DISK_FILESYSTEM_H
#define DISK_FILESYSTEM_H

#include <stdint.h>

// Virtual File System layer

namespace disk {
class Disk;
};

namespace filesystem {

class IFileSystem {
public:
    virtual bool Resolve(disk::Disk *disk) = 0;
};

void init_all();
IFileSystem *resolve(disk::Disk *disk);

}; // namespace filesystem

#endif // DISK_FILESYSTEM_H
