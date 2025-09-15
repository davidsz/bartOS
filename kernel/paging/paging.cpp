#include "paging.h"
#include "heap.h"
#include "status.h"

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
    if (!paging::is_aligned(virtual_address))
        return Status::E_INVALID_ARGUMENT;
    *directory_index_out = ((uint32_t)virtual_address / (TOTAL_ENTRIES_PER_DIRECTORY * PAGE_SIZE));
    *table_index_out = ((uint32_t) virtual_address % (TOTAL_ENTRIES_PER_TABLE * PAGE_SIZE) / PAGE_SIZE);
    return Status::ALL_OK;
}

namespace paging {

bool is_aligned(void *address)
{
    return (uint32_t)address % PAGE_SIZE == 0;
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

int set_table_entry(uint32_t *directory, void *v_address, uint32_t value)
{
    if (!is_aligned(v_address))
        return Status::E_INVALID_ARGUMENT;

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = get_indexes(v_address, &directory_index, &table_index);
    if (res < 0)
        return res;

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    table[table_index] = value;
    return Status::ALL_OK;
}

void enable()
{
    paging_enable();
}

}; // namespace paging
