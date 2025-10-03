#include "loader.h"
#include "config.h"
#include "disk/file.h"
#include "elf.h"
#include "elf_file.h"
#include "log.h"
#include "paging/paging.h"

namespace loader {

Binary *LoadFile(const String &filename)
{
    int fd = core::fopen(filename.c_str(), "r");
    if (fd < 0) {
        log::error("loader::LoadFile: Failed to open file %s (%d)\n", filename.c_str(), fd);
        return nullptr;
    }

    filesystem::FileStat stat;
    if (core::fstat(fd, &stat)) {
        log::error("loader::LoadFile: Failed to stat file %s\n", filename.c_str());
        core::fclose(fd);
        return nullptr;
    }

    void *program_data = calloc(stat.size);
    if (!program_data) {
        log::error("loader::LoadFile: Failed to allocate memory for %s\n", filename.c_str());
        core::fclose(fd);
        return nullptr;
    }

    if (core::fread(program_data, stat.size, 1, fd) != 1) {
        log::error("Process::LoadBinary: Failed to read file %s\n", filename.c_str());
        free(program_data);
        core::fclose(fd);
        return nullptr;
    }

    Binary *result = ELF_File::FromLoaded(filename, program_data, stat.size);
    if (!result)
        result = Binary::FromLoaded(filename, program_data, stat.size);
    return result;
}

int MapRawBinary(uint32_t *directory, Binary *binary)
{
    if (!binary->memoryAddress() || !binary->memorySize()) {
        log::error("Loader::MapRawBinary: Invalid argument\n");
        return Status::E_INVALID_ARGUMENT;
    }

    void *end_address = paging::align((void *)((uint32_t)binary->memoryAddress() + binary->memorySize()));
    uint8_t flags = paging::IS_PRESENT | paging::ACCESS_FROM_ALL | paging::IS_WRITEABLE;
    paging::map_from_to(directory, (void *)PROGRAM_VIRTUAL_ADDRESS, binary->memoryAddress(), end_address, flags);
    return Status::ALL_OK;
}

int MapElfBinary(uint32_t *directory, ELF_File *elf)
{
    elf_header *header = elf->elfHeader();
    elf32_phdr *program_header = (elf32_phdr *)((uint32_t)header + header->e_phoff);
    for (int i = 0; i < header->e_phnum; i++) {
        void *virtual_addr = paging::align_to_lower((void *)program_header->p_vaddr);
        void *physical_addr = (void *)((uint32_t)header + program_header->p_offset);
        void *physical_begin = paging::align_to_lower(physical_addr);
        void *physical_end = paging::align((void *)((uint32_t)physical_addr + program_header->p_memsz));
        int flags = paging::IS_PRESENT | paging::ACCESS_FROM_ALL;
        if (program_header->p_flags & PF_W)
            flags |= paging::IS_WRITEABLE;
        paging::map_from_to(directory, virtual_addr, physical_begin, physical_end, flags);
        program_header++;
    }
    return Status::ALL_OK;
}

}; // namespace loader
