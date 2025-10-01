#include "elf_file.h"
#include "elf.h"
#include "memory.h"

static const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

namespace loader {

ELF_File::ELF_File(const String &filename, void *program_data, size_t size)
    : Binary(filename, program_data, size)
{
    m_format = Binary::Format::Elf;
    m_header = (elf_header *)m_memoryAddress;
}

ELF_File::~ELF_File()
{
}

void *ELF_File::entryAddress()
{
    return (void *)m_header->e_entry;
}

bool ELF_File::HasValidFormat(void *buffer)
{
    return memcmp(buffer, (void *)elf_signature, sizeof(elf_signature)) == 0;
}

ELF_File *ELF_File::FromLoaded(const String &filename, void *program_data, size_t size)
{
    if (HasValidFormat(program_data))
        return new ELF_File(filename, program_data, size);
    return 0;
}

}; // namespace loader
