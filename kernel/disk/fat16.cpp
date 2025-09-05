#include "fat16.h"

namespace filesystem {

bool FAT16::Resolve(disk::Disk *)
{
    // TODO: Implement
    return true;
}

FileDescriptor *FAT16::Open(disk::Disk *, const Path &)
{
    // TODO: Implement
    return nullptr;
}

}; // namespace filesystem
