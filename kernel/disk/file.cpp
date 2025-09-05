#include "file.h"
#include "disk.h"
#include "filesystem.h"
#include "path.h"
#include "log.h"

namespace core {

static Vector<filesystem::FileDescriptor *> s_descriptors;

int fopen(const char *p, const char *m)
{
    // TODO: Handle modes
    if (*m != 'r') {
        log::error("fopen doesn't support mode '%c'", m);
        return E_INVALID_ARGUMENT;
    }

    Path path(p);
    if (!path.Valid()) {
        log::error("fopen failed to parse path '%s'", p);
        return Status::E_BAD_PATH;
    }

    disk::Disk *disk = disk::get_by_letter(path.DriveLetter());
    if (!disk) {
        log::error("fopen failed to find disk for '%c'", path.DriveLetter());
        return Status::E_BAD_PATH;
    }

    filesystem::FileDescriptor *fd = disk->fileSystem()->Open(disk, path);
    if (!fd) {
        log::error("fopen failed to open file '%s'", p);
        return Status::E_CANT_ACCESS;
    }

    auto it = s_descriptors.end();
    fd->id = s_descriptors.size() - 1;
    fd->path = move(path);
    fd->filesystem = disk->fileSystem();
    fd->disk = disk;
    s_descriptors.insert(it, fd);

    return fd->id;
}

};
