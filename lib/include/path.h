#ifndef LIB_PATH_H
#define LIB_PATH_H

#include "string.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>

class Path {
public:
    explicit Path(const char *path);
    const String &Data() const;
    bool Valid() const;
    char DriveLetter() const;
    const Vector<String> &Components() const;

    String &operator[](int index);
    const String &operator[](int index) const;
private:
    int Parse();

    String m_data;
    bool m_valid;
    Vector<String> m_parts;
    char m_driveLetter;
};

#endif // LIB_PATH_H
