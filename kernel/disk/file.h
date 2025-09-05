#ifndef DISK_FILE_H
#define DISK_FILE_H

#include <stdint.h>

// Virtual File System layer

namespace core {

int fopen(const char *path, const char *mode);
/*
int fclose(int fd);
int fseek(int fd, int offset, int whence);
int ftell(int fd);
int fread(void *ptr, int size, int count, int fd);
int fwrite(const void *ptr, int size, int count, int fd);
*/
}; // namespace core

#endif // DISK_FILE_H
