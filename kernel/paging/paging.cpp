#include "paging.h"
#include "heap.h"

// These are not optional
#define TOTAL_ENTRIES_PER_DIRECTORY 1024
#define TOTAL_ENTRIES_PER_TABLE 1024
#define PAGE_SIZE 4096

static uint32_t *s_currentDirectory = 0;

// Defined in paging.s
extern "C" void paging_load_directory(uint32_t *directory);
extern "C" void paging_enable();

namespace paging {

// TODO: Do we care about deallocation?
uint32_t *new_directory(uint8_t flags)
{
    // Allocate page directory
    // Memory blocks of kalloc are expected to be 4096 bytes aligned
    uint32_t *directory = (uint32_t *)kalloc(sizeof(uint32_t) * TOTAL_ENTRIES_PER_DIRECTORY);
    int offset = 0;
    for (int i = 0; i < TOTAL_ENTRIES_PER_DIRECTORY; i++) {
        // Each directory entry points to a page table
        uint32_t *entry = (uint32_t *)kalloc(sizeof(uint32_t) * TOTAL_ENTRIES_PER_TABLE);
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

void switch_directory(uint32_t *directory)
{
    paging_load_directory(directory);
    s_currentDirectory = directory;
}

void enable()
{
    paging_enable();
}

}; // namespace paging
