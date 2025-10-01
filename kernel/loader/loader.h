#ifndef LOADER_LOADER_H
#define LOADER_LOADER_H

#include "string.h"
#include <stdint.h>

namespace loader {

class Binary;
class ELF_File;

Binary *LoadFile(const String &filename);
int MapRawBinary(uint32_t *directory, Binary *binary);
int MapElfBinary(uint32_t *directory, ELF_File *elf);

}; // namespace loader

#endif //LOADER_LOADER_H
