#ifndef LIB_BLOCK_ALLOCATOR_H
#define LIB_BLOCK_ALLOCATOR_H

#include "heap.h"
#include <stdint.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE  0x00

#define HEAP_BLOCK_HAS_NEXT  0b10000000
#define HEAP_BLOCK_IS_FIRST  0b01000000

typedef uint8_t BlockTableEntry;

class BlockAllocator : public IAllocator
{
public:
    explicit BlockAllocator(size_t block_size);
    ~BlockAllocator();
    // IAllocator
    int Initialize(void *heap_start, size_t heap_size) override;
    void *Allocate(size_t bytes) override;
    void Deallocate(void *ptr) override;

private:
    bool ValidateAlignment(void *ptr);
    size_t AlignBytesToUpper(size_t bytes);
    void *AllocateBlocks(size_t total_blocks);
    size_t GetStartBlock(size_t total_blocks);
    int GetEntryType(BlockTableEntry entry);
    uint8_t *BlockToAddress(size_t block);
    size_t AddressToBlock(uint8_t *address);
    void MarkBlocksTaken(size_t start_block, size_t total_blocks);
    void MarkBlocksFree(size_t start_block);

    // Count of table entries
    size_t m_tableSize;
    // Heap size in bytes
    size_t m_heapSize;
    // Block size in bytes
    size_t m_blockSize;
    // Start address of the block table (at the start of the heap)
    BlockTableEntry *m_tableStartAddress;
    // Start address of the real heap (after the block table)
    uint8_t *m_heapStartAddress;
};

#endif // LIB_BLOCK_ALLOCATOR_H
