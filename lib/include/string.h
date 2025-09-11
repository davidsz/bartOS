#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <stddef.h>
#include "vector.h"

// TODO: Make it dynamic
class String {
public:
    String();
    String(const char *str);
    String(const char *str, size_t length);
    String(const String &str);
    String(String &&str);
    ~String();

    static String build(const char *format, ...);
    static String reset(char *str);

    String &operator=(const String &str);
    String &operator=(String &&str);

    bool operator==(const String &str) const;
    bool operator!=(const String &str) const;

    char &operator[](int index);
    const char &operator[](int index) const;

    const char *c_str() const;
    int length() const;

    Vector<String> split(char delimiter) const;

private:
    char *m_data;
    size_t m_length;
};

void strcpy(char *dest, const char *src);
int strlen(const char *ptr);
int strnlen(const char *ptr, int max);
bool isdigit(char c);
int tonumericdigit(char c);
char tolower(char c);
int strcmp(const char *s1, const char *s2);
int strcasecmp(const char *s1, const char *s2);

#endif // LIB_STRING_H
