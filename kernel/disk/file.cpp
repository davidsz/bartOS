#include "file.h"
#include "disk.h"
#include "filesystem.h"
#include "path.h"
#include "log.h"

namespace core {

static Vector<filesystem::FileDescriptor *> s_descriptors;

static filesystem::FileDescriptor *GetDescriptor(int fd)
{
    return s_descriptors[fd];
}

int fopen(const char *p, const char *m)
{
    // TODO: Handle modes
    if (*m != 'r') {
        log::error("fopen doesn't support mode '%c'\n", m);
        return E_NOT_IMPLEMENTED;
    }

    Path path(p);
    if (!path.valid()) {
        log::error("fopen failed to parse path '%s'\n", p);
        return Status::E_BAD_PATH;
    }

    disk::Disk *disk = disk::get_by_letter(path.driveLetter());
    if (!disk) {
        log::error("fopen failed to find disk for '%d'\n", path.driveLetter());
        return Status::E_BAD_PATH;
    }

    filesystem::FileDescriptor *fd = disk->fileSystem()->Open(disk, path);
    if (!fd) {
        log::error("fopen failed to open file '%s'\n", p);
        return Status::E_CANT_ACCESS;
    }

    auto it = s_descriptors.end();
    fd->id = s_descriptors.size();
    fd->path = move(path);
    fd->filesystem = disk->fileSystem();
    fd->disk = disk;
    s_descriptors.insert(it, fd);

    return fd->id;
}

size_t fread(void *buffer, size_t size, size_t count, uint32_t fd_id)
{
    if (size == 0 || count == 0)
        return 0;

    filesystem::FileDescriptor *fd = GetDescriptor(fd_id);
    if (!fd) {
        log::error("fread failed to get descriptor for ID %d", fd_id);
        return 0;
    }

    return fd->filesystem->Read(fd, size, count, (char *)buffer);
}

int fseek(uint32_t fd_id, size_t offset, filesystem::FileSeekMode whence)
{
    filesystem::FileDescriptor *fd = GetDescriptor(fd_id);
    if (!fd) {
        log::error("fseek failed to get descriptor for ID %d", fd_id);
        return Status::E_INVALID_ARGUMENT;
    }

    return fd->filesystem->Seek(fd, offset, whence);
}

int fstat(uint32_t fd_id, filesystem::FileStat *stat)
{
    filesystem::FileDescriptor *fd = GetDescriptor(fd_id);
    if (!fd) {
        log::error("fstat failed to get descriptor for ID %d", fd_id);
        return Status::E_INVALID_ARGUMENT;
    }

    return fd->filesystem->Stat(fd, stat);
}

int fclose(int fd_id)
{
    filesystem::FileDescriptor *fd = GetDescriptor(fd_id);
    if (!fd) {
        log::error("fclose failed to get descriptor for ID %d", fd_id);
        return Status::E_INVALID_ARGUMENT;
    }

    if (fd->filesystem->Close(fd))
        delete fd;
    return 0;
}

}; // namespace core
