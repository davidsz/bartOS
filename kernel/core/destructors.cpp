#include <stddef.h>

struct DestructorEntry {
    void (*destructor)(void *);
    void *arg;
    void *dso_handle;
};

// TODO: Make this dynamic
const size_t MAX_DESTRUCTORS = 128;
DestructorEntry s_destructorList[MAX_DESTRUCTORS];
static size_t s_destructorCount = 0;

// GCC calls it automatically
extern "C" int __cxa_atexit(void (*destructor)(void *), void *arg, void *dso_handle) {
    if (s_destructorCount >= MAX_DESTRUCTORS)
        return -1;
    s_destructorList[s_destructorCount++] = { destructor, arg, dso_handle };
    return 0;
}

extern "C" void __cxa_finalize(void *dso_handle) {
    // Calling destructors in reverse order
    for (int i = s_destructorCount - 1; i >= 0; i--) {
        DestructorEntry &entry = s_destructorList[i];
        if (entry.destructor && (!dso_handle || entry.dso_handle == dso_handle)) {
            entry.destructor(entry.arg);
            s_destructorList[i] = {};
        }
    }
}
