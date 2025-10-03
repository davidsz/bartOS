#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include "stddef.h"

void print(const char *message);
void *memcpy(void *dest, const void *src, size_t size);

#endif // LIBC_STDIO_H
