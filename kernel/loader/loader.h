#ifndef LOADER_LOADER_H
#define LOADER_LOADER_H

#include "string.h"

namespace loader {

class Binary;

Binary *LoadFile(const String &filename);

}; // namespace loader

#endif //LOADER_LOADER_H
