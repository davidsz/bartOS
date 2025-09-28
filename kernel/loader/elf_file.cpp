#include "elf_file.h"
#include "elf.h"

namespace loader {

ELF_File::ELF_File(const String &filename, void *program_data, size_t size)
    : Binary(filename, program_data, size)
{
}

ELF_File::~ELF_File()
{
}

bool ELF_File::HasValidFormat(void *)
{
    return false;
}

ELF_File *ELF_File::FromLoaded(const String &filename, void *program_data, size_t size)
{
    ELF_File *ret = new ELF_File(filename, program_data, size);
    ret->m_format = Binary::Format::Elf;
    return ret;
}

}; // namespace loader
