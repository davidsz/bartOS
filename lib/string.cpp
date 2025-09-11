#include "string.h"
#include "log.h"
#include "memory.h"
#include "vargs.h"

String::String()
{
    m_length = 0;
    m_data = new char[1];
    m_data[0] = '\0';
}

String::String(const char *str)
{
    m_length = strlen(str);
    m_data = new char[m_length + 1];
    memcpy(m_data, str, m_length + 1);
}

String::String(const char *str, size_t length)
{
    m_length = length + 1;
    m_data = new char[m_length];
    memcpy(m_data, str, length);
    m_data[length] = '\0';
}

String::String(const String &str)
{
    m_length = str.m_length;
    m_data = new char[m_length + 1];
    memcpy(m_data, str.m_data, m_length + 1);
}

String::String(String &&str)
{
    m_length = str.m_length;
    str.m_length = 0;
    m_data = str.m_data;
    delete[] str.m_data;
}

String::~String()
{
    delete[] m_data;
}

String String::build(const char *format, ...)
{
    VA_LIST args;

    size_t buffer_size = 0;
    // Determine the overall size
    VA_START(args, format);
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            switch(format[i + 1]) {
            // String
            case 's': {
                char *s = VA_ARG(args, char *);
                buffer_size += strlen(s);
                i++;
                continue;
            }
            default: {
                log::error("Unrecognized format placeholder: %c\n", format[i + 1]);
                return String();
            }
            }
        }
        buffer_size++;
    }
    VA_END(args);

    char *buffer = new char[buffer_size];
    size_t buffer_pos = 0;
    // Compose the string
    VA_START(args, format);
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            switch(format[i + 1]) {
            // String
            case 's': {
                char *s = VA_ARG(args, char *);
                while (*s != '\0')
                    buffer[buffer_pos++] = *s++;
                i++;
                continue;
            }
            default: {} // We have handled unrecognized format placeholders above
            }
        }
        buffer[buffer_pos++] = format[i];
    }
    buffer[buffer_pos++] = '\0';
    VA_END(args);

    String ret;
    ret.m_data = buffer;
    ret.m_length = buffer_pos;
    return ret;
}

String String::reset(char *str)
{
    String ret;
    ret.m_data = str;
    ret.m_length = strlen(str);
    return ret;
}

String &String::operator=(const String &str)
{
    if (this != &str) {
        delete[] m_data;
        m_length = str.m_length;
        m_data = new char[m_length + 1];
        memcpy(m_data, str.m_data, m_length + 1);
    }
    return *this;
}

String &String::operator=(String &&str)
{
    if (this != &str) {
        delete[] m_data;
        m_length = str.m_length;
        str.m_length = 0;
        m_data = str.m_data;
        delete[] str.m_data;
    }
    return *this;
}

bool String::operator==(const String &str) const
{
    if (m_length != str.m_length)
        return false;
    for (size_t i = 0; i < m_length; i++) {
        if (m_data[i] != str.m_data[i])
            return false;
    }
    return true;
}

bool String::operator!=(const String &str) const
{
    return !(*this == str);
}

char &String::operator[](int index)
{
    return m_data[index];
}

const char &String::operator[](int index) const
{
    return m_data[index];
}

const char *String::c_str() const
{
    return m_data;
}

int String::length() const
{
    return m_length;
}

Vector<String> String::split(char delimiter) const
{
    Vector<String> result;
    size_t start = 0;
    for (size_t i = 0; i < m_length - 1; i++) {
        if (m_data[i] == delimiter) {
            result.push_back(String(m_data + start, i - start));
            start = i + 1;
        }
    }
    if (start < m_length - 1)
        result.push_back(String(m_data + start, m_length - start));
    return result;
}

void strcpy(char *dest, const char *src)
{
    memcpy(dest, src, strlen(src) + 1);
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

bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

int tonumericdigit(char c)
{
    return c - 48;
}

char tolower(char c)
{
    if (c >= 65 && c <= 90)
        c += 32;
    return c;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

int strcasecmp(const char *s1, const char *s2)
{
    char c1, c2;
    while (*s1 && *s2) {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
        if (c1 != c2)
            return c1 - c2;
    }
    c1 = tolower(*s1);
    c2 = tolower(*s2);
    return c1 - c2;
}
