#ifndef LIB_MEMORY_H
#define LIB_MEMORY_H

#include <stddef.h>

int memcmp(const void *a, const void *b, size_t count);
void memcpy(void *dest, const void *src, size_t size);
void *memset(void *ptr, int c, size_t size);

#endif // LIB_MEMORY_H
