#ifndef LOADER_BINARY_H
#define LOADER_BINARY_H

#include "string.h"

namespace loader {

class Binary
{
public:
    enum Format {
        RawBinary,
        Elf,
    };

    static Binary *FromLoaded(const String &filename, void *program_data, size_t size);

    virtual ~Binary();
    virtual void *entryAddress();

    String filename() { return m_filename; }
    Format format() { return m_format; }
    void *memoryAddress() { return m_memoryAddress; }
    size_t memorySize() { return m_memorySize; }

protected:
    Binary(const String &filename, void *program_data, size_t size);

    String m_filename;
    // The physical memory address that this executable file is loaded at
    void *m_memoryAddress = 0;
    int m_memorySize = 0;
    Format m_format;
};

}; // namespace loader

#endif //LOADER_BINARY_H
