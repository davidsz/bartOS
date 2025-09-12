#ifndef DISK_FILE_H
#define DISK_FILE_H

#include <stddef.h>
#include <stdint.h>

// Virtual File System layer

namespace filesystem {

enum FileSeekMode {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

}; // namespace filesystem

namespace core {

int fopen(const char *path, const char *mode);
size_t fread(void *buffer, size_t size, size_t count, uint32_t fd);
int fseek(uint32_t fd, size_t offset, filesystem::FileSeekMode whence);
int fclose(int fd);

}; // namespace core

#endif // DISK_FILE_H
