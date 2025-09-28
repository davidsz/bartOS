#ifndef LOADER_ELF_FILE_H
#define LOADER_ELF_FILE_H

#include "binary.h"

namespace loader {

class ELF_File : public Binary
{
public:
    static ELF_File *FromLoaded(const String &filename, void *program_data, size_t size);
    static bool HasValidFormat(void *program_data);

    virtual ~ELF_File();

private:
    ELF_File(const String &filename, void *program_data, size_t size);

    void *m_virtualBaseAddress;
    void *m_virtualEndAddress;
    void *m_physicalBaseAddress;
    void *m_physicalEndAddress;
};

} // namespace loader

#endif // LOADER_ELF_FILE_H
