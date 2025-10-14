#ifndef OS_KERNEL_H
#define OS_KERNEL_H

#include "paging/paging.h"

namespace kernel {

paging::Directory page_directory();
void panic(const char *message);

}; // namespace kernel

#endif // OS_KERNEL_H
