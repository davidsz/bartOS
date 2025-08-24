#ifndef LIB_MEMORY_H
#define LIB_MEMORY_H

#include <stddef.h>
#include <stdint.h>

int memcmp(const void *a, const void *b, size_t count);
void memcpy(void *dest, const void *src, size_t size);
void *memset(void *ptr, uint8_t byte, size_t size);

#endif // LIB_MEMORY_H
