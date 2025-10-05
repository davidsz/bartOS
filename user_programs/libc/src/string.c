#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

char *strcpy(char *dest, const char *src)
{
    memcpy(dest, src, strlen(src) + 1);
    return 0;
}

char *strncpy(char *dest, const char *src, int count)
{
    int i = 0;
    for (i = 0; i < count - 1; i++) {
        if (src[i] == 0x00)
            break;
        dest[i] = src[i];
    }
    dest[i] = 0x00;
    return dest;
}

int strlen(const char *ptr)
{
    int i = 0;
    while(*ptr++ != 0)
        i++;
    return i;
}

int strnlen(const char *ptr, int max)
{
    int i = 0;
    for (i = 0; i < max; i++) {
        if (ptr[i] == 0)
            break;
    }
    return i;
}

int strnlen_terminator(const char *str, int max, char terminator)
{
    int i = 0;
    for(i = 0; i < max; i++) {
        if (str[i] == '\0' || str[i] == terminator)
            break;
    }
    return i;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

int istrncmp(const char *s1, const char *s2, int n)
{
    unsigned char u1, u2;
    while(n-- > 0) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2))
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }
    return 0;
}

int strncmp(const char *str1, const char *str2, int n)
{
    unsigned char u1, u2;
    while(n-- > 0) {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }
    return 0;
}

char *strtok(char *s, char d)
{
    static char *input = NULL;

    if (s != NULL)
        input = s;
    if (input == NULL)
        return NULL;

    while (*input == d)
        input++;

    if (*input == '\0') {
        input = NULL;
        return NULL;
    }

    char *start = input;

    while (*input != '\0' && *input != d)
        input++;

    if (*input == d) {
        *input = '\0';
        input++;
    } else {
        input = NULL;
    }

    return start;
}
