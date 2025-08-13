#ifndef LIB_HEAP_H
#define LIB_HEAP_H

#include "status.h"
#include <stddef.h>

class IAllocator
{
public:
    virtual int Initialize(void *heap_start, size_t heap_size) = 0;
    virtual void *Allocate(size_t bytes) = 0;
    virtual void Deallocate(void *ptr) = 0;
};

int set_heap_allocator(IAllocator *allocator);

// C-style allocation
void *malloc(size_t size);
void *kalloc(size_t size);
void free(void *ptr);

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
