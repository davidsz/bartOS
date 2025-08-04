#ifndef CORE_CONSTRUCTORS_H
#define CORE_CONSTRUCTORS_H

// Global constructor handling

namespace core {

// Call all constructors of global objects. Without this, objects
// in the global scope won't work.
// Requires the support of the linker script.
void call_constructors();

}; // namespace core

#endif // CORE_CONSTRUCTORS_H
