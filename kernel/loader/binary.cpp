#include "binary.h"
#include "heap.h"

namespace loader {

Binary::Binary(const String &filename, void *program_data, size_t size)
    : m_filename(filename)
    , m_memoryAddress(program_data)
    , m_memorySize(size)
{
}

Binary::~Binary()
{
    free(m_memoryAddress);
}

Binary *Binary::FromLoaded(const String &filename, void *program_data, size_t size)
{
    Binary *ret = new Binary(filename, program_data, size);
    ret->m_format = Format::RawBinary;
    return ret;
}

}; // namespace loader
