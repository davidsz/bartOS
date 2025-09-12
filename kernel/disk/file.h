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

enum FileStatFlag {
    ReadOnly = 0b00000001
};

struct FileStat {
    uint32_t size = 0;
    uint8_t flags = 0;
};

}; // namespace filesystem

namespace core {

int fopen(const char *path, const char *mode);
size_t fread(void *buffer, size_t size, size_t count, uint32_t fd);
int fseek(uint32_t fd, size_t offset, filesystem::FileSeekMode whence);
int fstat(uint32_t fd, filesystem::FileStat *stat);
int fclose(int fd);

}; // namespace core

#endif // DISK_FILE_H
