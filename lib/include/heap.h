#ifndef LIB_HEAP_H
#define LIB_HEAP_H

#include <stddef.h>

class IAllocator;

int set_heap_allocator(IAllocator *allocator);

// C-style allocation
void *kmalloc(size_t size);
void kfree(void *ptr);

// New and delete
void *operator new(size_t size);
void operator delete(void *ptr) noexcept;

void *operator new[](size_t size);
void operator delete[](void *ptr) noexcept;

// Placement new and delete
void *operator new(size_t size, void *ptr) noexcept;
void operator delete(void *ptr, void *place) noexcept;

void *operator new[](size_t size, void *ptr) noexcept;
void operator delete[](void *ptr, void *place) noexcept;

// Sized deallocations
void operator delete(void *ptr, size_t size) noexcept;
void operator delete[](void *ptr, size_t size) noexcept;

#endif // LIB_HEAP_H
