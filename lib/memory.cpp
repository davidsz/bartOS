#include "memory.h"

int memcmp(const void *a, const void *b, size_t count)
{
    char *c1 = (char *)a;
    char *c2 = (char *)b;
    while(count-- > 0) {
        if (*c1++ != *c2++)
            return c1[-1] < c2[-1] ? -1 : 1;
    }
    return 0;
}

void memcpy(void *dest, const void *src, size_t size)
{
    for (unsigned int i = 0; i < size; i++)
        ((char *)dest)[i] = ((char *)src)[i];
}

void *memset(void *ptr, int c, size_t size)
{
    for (unsigned int i = 0; i < size; i++)
        ((char *)ptr)[i] = (char)c;
    return ptr;
}
