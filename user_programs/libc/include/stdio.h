#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stdbool.h>
#include <stddef.h>

int putchar(int c);
int puts(const char *message);
int printf(const char *fmt, ...);

int getkey();
void readline(char *out, int max, bool output_while_typing);

void *memset(void *ptr, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t size);
int memcmp(const void *str1, const void *str2, size_t n);

#endif // LIBC_STDIO_H
