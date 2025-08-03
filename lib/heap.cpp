#include "include/heap.h"
#include "include/iallocator.h"
#include "include/status.h"
#include <stdint.h>

namespace {
IAllocator *s_allocator;
}

int set_heap_allocator(IAllocator *allocator)
{
    if (!allocator)
        return Status::EINVARG;
    s_allocator = allocator;
    return Status::ALL_OK;
}

void *kmalloc(size_t size)
{
    if (!s_allocator)
        return 0;
    return s_allocator->Allocate(size);
}

void kfree(void *ptr)
{
    if (!s_allocator)
        return;
    s_allocator->Deallocate(ptr);
}

void *operator new(size_t size) {
    return kmalloc(size);
}

void operator delete(void *ptr) noexcept {
    kfree(ptr);
}

void *operator new[](size_t size) {
    return kmalloc(size);
}

void operator delete[](void *ptr) noexcept {
    kfree(ptr);
}

void *operator new(size_t, void *ptr) noexcept {
    return ptr;
}

void operator delete(void*, void*) noexcept {
}

void *operator new[](size_t, void *ptr) noexcept {
    return ptr;
}

void operator delete[](void*, void*) noexcept {
}

void operator delete(void *, size_t) noexcept {
}

void operator delete[](void *, size_t) noexcept {
}
