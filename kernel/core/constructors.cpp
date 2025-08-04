#include "constructors.h"

namespace core {

typedef void (*constructor)();

// Defined in the linker script
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

void call_constructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

}; // namespace core
