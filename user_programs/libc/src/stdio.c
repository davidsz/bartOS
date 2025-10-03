#include "stdio.h"

void *memcpy(void *dest, const void *src, size_t size)
{
    for (unsigned int i = 0; i < size; i++)
        ((char *)dest)[i] = ((char *)src)[i];
    return 0;
}
