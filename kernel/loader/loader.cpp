#include "loader.h"
#include "disk/file.h"
#include "log.h"
#include "elf_file.h"

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

    Binary *result = 0;
    if (ELF_File::HasValidFormat(program_data))
        result = ELF_File::FromLoaded(filename, program_data, stat.size);
    else
        result = Binary::FromLoaded(filename, program_data, stat.size);

    return result;
}

}; // namespace loader
