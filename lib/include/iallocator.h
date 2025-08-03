#ifndef LIB_IALLOCATOR_H
#define LIB_IALLOCATOR_H

#include "status.h"
#include <stddef.h>

class IAllocator
{
public:
    virtual int Initialize(void *heap_start, size_t heap_size) = 0;
    virtual void *Allocate(size_t bytes) = 0;
    virtual void Deallocate(void *ptr) = 0;
};

#endif // LIB_IALLOCATOR_H
