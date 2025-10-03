#include "cstring.h"
#include "stdio.h"

void *strcpy(char *dest, const char *src)
{
    memcpy(dest, src, strlen(src) + 1);
    return 0;
}

int strlen(const char *ptr)
{
    int i = 0;
    while(*ptr++ != 0)
        i++;
    return i;
}
