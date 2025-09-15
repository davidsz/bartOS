#ifndef PAGING_PAGING_H
#define PAGING_PAGING_H

#include <stdbool.h>
#include <stdint.h>

// Implementation of paging and virtual memory

namespace paging {

using Directory = uint32_t *;

enum Flags : uint8_t {
    CACHE_DISABLED  = 0b00010000,
    WRITE_THROUGH   = 0b00001000,
    ACCESS_FROM_ALL = 0b00000100,
    IS_WRITEABLE    = 0b00000010,
    IS_PRESENT      = 0b00000001
};

bool is_aligned(void *address);
uint32_t *new_directory(uint8_t flags);
void free_directory(uint32_t * directory);
void switch_directory(uint32_t *directory);
int set_table_entry(uint32_t *directory, void *v_address, uint32_t value);
void enable();

}; // namespace paging

#endif // PAGING_PAGING_H
