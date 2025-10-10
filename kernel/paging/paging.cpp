#include "heap.h"
#include "log.h"
#include "paging.h"
#include "status.h"
#include <cstdint>

// Structure of virtual addresses:
// bit 31-22: Page directory index
// bit 21-12: Page table index
// bit 11-0:  Offset in page

// Page directory entry (32 bits):
// 7-4 bytes: Page table address
// 3-0 bytes: Flags

// Page table entry (32 bits):
// 7-4 bytes: Physical address
// 3-0 bytes:  Flags

// These are not optional
#define TOTAL_ENTRIES_PER_DIRECTORY 1024
#define TOTAL_ENTRIES_PER_TABLE 1024
#define PAGE_SIZE 4096

static uint32_t *s_currentDirectory = 0;

// Defined in paging.s
extern "C" void paging_load_directory(uint32_t *directory);
extern "C" void paging_enable();

static int get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    if (!paging::is_aligned(virtual_address)) {
        log::error("paging::get_indexes: virtual_address must be aligned\n");
        return Status::E_INVALID_ARGUMENT;
    }
    *directory_index_out = ((uint32_t)virtual_address / (TOTAL_ENTRIES_PER_DIRECTORY * PAGE_SIZE));
    *table_index_out = ((uint32_t) virtual_address % (TOTAL_ENTRIES_PER_TABLE * PAGE_SIZE) / PAGE_SIZE);
    return Status::ALL_OK;
}

namespace paging {

bool is_aligned(void *address)
{
    return (uint32_t)address % PAGE_SIZE == 0;
}

void *align(void *address)
{
    if (!is_aligned(address))
        return (void *)((uint32_t)address + PAGE_SIZE - ((uint32_t)address % PAGE_SIZE));
    return address;
}

void *align_to_lower(void *address)
{
    return (void *)((uint32_t)address - (uint32_t)address % PAGE_SIZE);
}

uint32_t *new_directory(uint8_t flags)
{
    // Allocate page directory
    // Memory blocks of calloc are expected to be 4096 bytes aligned
    uint32_t *directory = (uint32_t *)calloc(sizeof(uint32_t) * TOTAL_ENTRIES_PER_DIRECTORY);
    int offset = 0;
    for (int i = 0; i < TOTAL_ENTRIES_PER_DIRECTORY; i++) {
        // Each directory entry points to a page table
        uint32_t *entry = (uint32_t *)calloc(sizeof(uint32_t) * TOTAL_ENTRIES_PER_TABLE);
        // Fill the page table with entries
        // Physical address | flags
        for (int b = 0; b < TOTAL_ENTRIES_PER_TABLE; b++)
            entry[b] = (offset + (b * PAGE_SIZE)) | flags;
        // The next page table will cover the next 4 Mb
        offset += (TOTAL_ENTRIES_PER_TABLE * PAGE_SIZE);
        // Add the flags to the directory entry and store it
        directory[i] = (uint32_t)entry | flags | IS_WRITEABLE;
    }
    return directory;
}

void free_directory(uint32_t *directory)
{
    for (int i = 0; i < TOTAL_ENTRIES_PER_DIRECTORY; i++) {
        uint32_t entry = directory[i];
        uint32_t *table = (uint32_t *)(entry & 0xfffff000);
        free(table);
    }
    free(directory);
}

void switch_directory(uint32_t *directory)
{
    paging_load_directory(directory);
    s_currentDirectory = directory;
}

void map(uint32_t *directory, void *virt, void *phys, int flags)
{
    set_table_entry(directory, virt, (uint32_t)phys | flags);
}

void map_range(uint32_t *directory, void *virt, void *phys, size_t page_count, int flags)
{
    if (!is_aligned(virt) || !is_aligned(phys)) {
        log::error("paging::map_range: Addresses must be aligned\n");
        return;
    }

    uint32_t physical_address = (uint32_t)phys;
    uint32_t virtual_address = (uint32_t)virt;
    for (size_t i = 0; i < page_count; i++) {
        map(directory, (void *)virtual_address, (void *)physical_address, flags);
        virtual_address += PAGE_SIZE;
        physical_address += PAGE_SIZE;
    }
}

void map_from_to(uint32_t *directory, void *virt, void *phys, void *phys_end, uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys) || !is_aligned(phys_end)) {
        log::error("paging::map_from_to: Addresses must be aligned\n");
        return;
    }

    if ((uint32_t)phys_end < (uint32_t)phys) {
        log::error("paging::map_from_to: End address must be greater than start address\n");
        return;
    }

    uint32_t total_bytes = (uint32_t)phys_end - (uint32_t)phys;
    int total_pages = total_bytes / PAGE_SIZE;
    map_range(directory, virt, phys, total_pages, flags);
}

void set_table_entry(uint32_t *directory, void *virt, uint32_t value)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    if (get_indexes(virt, &directory_index, &table_index) != Status::ALL_OK) {
        log::error("paging::set_table_entry: Could not get indexes\n");
        return;
    }

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    table[table_index] = value;
}

uint32_t get_table_entry(uint32_t *directory, void *virt)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    if (get_indexes(virt, &directory_index, &table_index) != Status::ALL_OK) {
        log::error("paging::get_table_entry: Could not get indexes\n");
        return 0;
    }

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    return table[table_index];
}

void *get_physical_address(uint32_t *directory, void *virt)
{
    void *virt_addr_aligned = align_to_lower(virt);
    uint32_t difference = (uint32_t)virt - (uint32_t)virt_addr_aligned;
    return (void *)((get_table_entry(directory, virt_addr_aligned) & 0xfffff000) + difference);
}

void enable()
{
    paging_enable();
}

}; // namespace paging
