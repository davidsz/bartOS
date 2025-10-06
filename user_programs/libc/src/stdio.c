#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static char *tohex32(uint32_t h)
{
    char *res = (char *)malloc(10 * sizeof(char));
    strcpy(res, "0x00000000");
    const char *charset = "0123456789ABCDEF";
    for (int i = 2, shift = 28; i < 10; i++, shift -= 4)
        res[i] = charset[(h >> shift) & 0xF];
    return res;
}

int printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    const char *p;
    for (p = format; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }

        switch (*++p) {
        case 'c': {
            putchar(va_arg(ap, char));
            break;
        }
        case 'i':
        case 'd': {
            puts(itoa(va_arg(ap, int)));
            break;
        }
        case 's': {
            puts(va_arg(ap, char *));
            break;
        }
        case 'p': {
            char *hex = tohex32(va_arg(ap, uint32_t));
            puts(hex);
            free(hex);
            break;
        }
        default: {
            putchar(*p);
            break;
        }
        }
    }
    va_end(ap);
    return 0;
}

void readline(char *out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max -1; i++) {
        // Get key
        char key = 0;
        do {
            key = getkey();
        } while(key == 0);

        // Carriage return means we have read the line
        if (key == 13)
            break;

        if (output_while_typing)
            putchar(key);

        // Backspace
        if (key == 0x08 && i >= 1) {
            out[i - 1] = 0x00;
            i -= 2;
            continue;
        }
        out[i] = key;
    }

    // Add the null terminator
    out[i] = 0x00;
}

void *memset(void *ptr, int c, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((uint8_t *)ptr)[i] = c;
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t size)
{
    for (unsigned int i = 0; i < size; i++)
        ((char *)dest)[i] = ((char *)src)[i];
    return 0;
}

int memcmp(const void *str1, const void *str2, size_t n)
{
    char *c1 = (char *)str1;
    char *c2 = (char *)str2;
    while(n-- > 0) {
        if (*c1++ != *c2++)
            return c1[-1] < c2[-1] ? -1 : 1;
    }
    return 0;
}
