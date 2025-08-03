#ifndef CORE_DESTRUCTORS_H
#define CORE_DESTRUCTORS_H

// Global destructor handling

extern "C" {

// We let the compiler handle the registration of global destructors
// by __cxa_atexit. It could be prevented by -fno-use-cxa-atexit,
// in that case the .dtros secton is available for use.
// - destructor: function pointer
// - arg: typically a pointer to the object that is being destroyed
// - dso_handle: identifier for the Dynamic Shared Object
//               that is being destroyed
int __cxa_atexit(void (*destructor)(void *), void *arg, void *dso_handle);

// Call all registered destructors for a given DSO handle
void __cxa_finalize(void *dso_handle);

}

#endif // CORE_DESTRUCTORS_H
