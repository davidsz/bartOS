#ifndef PAGING_PAGING_H
#define PAGING_PAGING_H

#include <stdbool.h>
#include <stddef.h>
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
void *align(void *address);
void *align_to_lower(void *address);

// TODO: Directory can be a class
uint32_t *new_directory(uint8_t flags);
void free_directory(uint32_t * directory);
void switch_directory(uint32_t *directory);
void map_from_to(uint32_t *directory, void *virt, void *phys, void *phys_end, uint8_t flags);
void map_range(uint32_t *directory, void *virt, void *phys, size_t page_count, int flags);
void map(uint32_t *directory, void *virt, void *phys, int flags);
void set_table_entry(uint32_t *directory, void *virt, uint32_t value);
uint32_t get_table_entry(uint32_t *directory, void *virt);

void enable();

}; // namespace paging

#endif // PAGING_PAGING_H
