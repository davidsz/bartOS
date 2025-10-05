#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <stdbool.h>

char *strcpy(char *dest, const char *src);
char *strncpy(char* dest, const char* src, int count);

int strlen(const char *ptr);
int strnlen(const char *ptr, int max);
int strnlen_terminator(const char *str, int max, char terminator);

int strcmp(const char *s1, const char *s2);
int istrncmp(const char *s1, const char *s2, int n);
int strncmp(const char *str1, const char *str2, int n);

char *strtok(char *str, char delimiter);

#endif // LIBC_STRING_H
