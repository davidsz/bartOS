#ifndef DISK_FILE_H
#define DISK_FILE_H

#include <stddef.h>
#include <stdint.h>

// Virtual File System layer

namespace core {

int fopen(const char *path, const char *mode);
int fread(void *buffer, size_t size, size_t count, uint32_t fd);
/*
int fclose(int fd);
int fseek(int fd, int offset, int whence);
int ftell(int fd);
int fwrite(const void *ptr, int size, int count, int fd);
*/
}; // namespace core

#endif // DISK_FILE_H
