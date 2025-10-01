#include "binary.h"
#include "heap.h"
#include "config.h"

namespace loader {

Binary::Binary(const String &filename, void *program_data, size_t size)
    : m_filename(filename)
    , m_memoryAddress(program_data)
    , m_memorySize(size)
    , m_format(Binary::Format::RawBinary)
{
}

Binary::~Binary()
{
    free(m_memoryAddress);
}

void *Binary::entryAddress()
{
    return (void *)PROGRAM_VIRTUAL_ADDRESS;
}

Binary *Binary::FromLoaded(const String &filename, void *program_data, size_t size)
{
    return new Binary(filename, program_data, size);
}

}; // namespace loader
