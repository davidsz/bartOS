#ifndef LOADER_ELF_FILE_H
#define LOADER_ELF_FILE_H

#include "binary.h"

namespace loader {

struct elf_header;

class ELF_File : public Binary
{
public:
    static ELF_File *FromLoaded(const String &filename, void *program_data, size_t size);
    static bool HasValidFormat(void *program_data);

    virtual ~ELF_File();

    void *entryAddress() override;
    elf_header *elfHeader() { return m_header; }

private:
    ELF_File(const String &filename, void *program_data, size_t size);

    elf_header *m_header;
};

} // namespace loader

#endif // LOADER_ELF_FILE_H
