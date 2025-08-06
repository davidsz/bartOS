#include "include/heap.h"
#include "include/iallocator.h"
#include "include/memory.h"
#include "include/status.h"
#include <stdint.h>

static IAllocator *s_allocator = nullptr;

int set_heap_allocator(IAllocator *allocator)
{
    if (!allocator)
        return Status::EINVARG;
    s_allocator = allocator;
    return Status::ALL_OK;
}

void *malloc(size_t size)
{
    if (!s_allocator)
        return 0;
    return s_allocator->Allocate(size);
}

void *kalloc(size_t size)
{
    if (void *ptr = malloc(size)) {
        memset(ptr, 0x00, size);
        return ptr;
    }
    return 0;
}

void free(void *ptr)
{
    if (!s_allocator)
        return;
    s_allocator->Deallocate(ptr);
}

void *operator new(size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) noexcept {
    free(ptr);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete[](void *ptr) noexcept {
    free(ptr);
}

void *operator new(size_t, void *ptr) noexcept {
    return ptr;
}

void operator delete(void *, void *) noexcept {
}

void *operator new[](size_t, void *ptr) noexcept {
    return ptr;
}

void operator delete[](void *, void *) noexcept {
}

void operator delete(void *, size_t) noexcept {
}

void operator delete[](void *, size_t) noexcept {
}
