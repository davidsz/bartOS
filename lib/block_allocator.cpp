#include "block_allocator.h"
#include "memory.h"
#include "status.h"

BlockAllocator::BlockAllocator(size_t block_size)
    : m_blockSize(block_size)
{
}

BlockAllocator::~BlockAllocator()
{
    memset(m_tableStartAddress, 0, m_tableSize);
}

int BlockAllocator::Initialize(void *heap_start, size_t heap_size)
{
    if (!ValidateAlignment(heap_start) || !ValidateAlignment((uint8_t *)heap_start + heap_size))
        return Status::E_INVALID_ARGUMENT;

    m_heapSize = heap_size;
    m_tableSize = heap_size / m_blockSize;
    m_tableStartAddress = (BlockTableEntry *)heap_start;
    m_heapStartAddress = (uint8_t *)heap_start + AlignBytesToUpper(m_tableSize);

    if (!ValidateAlignment(m_heapStartAddress))
        return Status::E_INVALID_ARGUMENT;

    memset(m_tableStartAddress, HEAP_BLOCK_TABLE_ENTRY_FREE, m_tableSize);

    return Status::ALL_OK;
}

uint8_t *BlockAllocator::BlockToAddress(size_t block)
{
    return m_heapStartAddress + (block * m_blockSize);
}

size_t BlockAllocator::AddressToBlock(uint8_t *address)
{
    return (address - m_heapStartAddress) / m_blockSize;
}

int BlockAllocator::GetEntryType(BlockTableEntry entry)
{
    return entry & 0x0f;
}

size_t BlockAllocator::GetStartBlock(size_t total_blocks)
{
    size_t bc = 0;
    int bs = -1;
    for (size_t i = 0; i < m_tableSize; i++) {
        if (GetEntryType(m_tableStartAddress[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }

        // If this is the first block
        if (bs == -1)
            bs = i;
        bc++;
        if (bc == total_blocks)
            break;
    }

    if (bs == -1)
        return Status::E_NO_MEMORY;
    return bs;
}

void BlockAllocator::MarkBlocksTaken(size_t start_block, size_t total_blocks)
{
    size_t end_block = (start_block + total_blocks) - 1;

    BlockTableEntry entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1)
        entry |= HEAP_BLOCK_HAS_NEXT;

    for (size_t i = start_block; i <= end_block; i++) {
#if 0
        // Breakpoint for debugging of heap corruptions
        if (m_tableStartAddress[i] & HEAP_BLOCK_TABLE_ENTRY_TAKEN)
            return;
#endif
        m_tableStartAddress[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1)
            entry |= HEAP_BLOCK_HAS_NEXT;
    }
};

void BlockAllocator::MarkBlocksFree(size_t start_block)
{
    for (size_t i = start_block; i < m_tableSize; i++) {
#if 0
        // Breakpoint for debugging of heap corruptions
        if (m_tableStartAddress[i] == HEAP_BLOCK_TABLE_ENTRY_FREE)
            return;
#endif
        BlockTableEntry entry = m_tableStartAddress[i];
        m_tableStartAddress[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT))
            break;
    }
}

void *BlockAllocator::AllocateBlocks(size_t total_blocks)
{
    int start_block = GetStartBlock(total_blocks);
    if (start_block < 0)
        return 0;

    MarkBlocksTaken(start_block, total_blocks);
    return BlockToAddress(start_block);
}

void *BlockAllocator::Allocate(size_t bytes)
{
    size_t aligned_size = AlignBytesToUpper(bytes);
    size_t total_blocks = aligned_size / m_blockSize;
    return AllocateBlocks(total_blocks);
}

void BlockAllocator::Deallocate(void *ptr)
{
    MarkBlocksFree(AddressToBlock((uint8_t *)ptr));
}

bool BlockAllocator::ValidateAlignment(void *ptr)
{
    return (uintptr_t)ptr % m_blockSize == 0;
}

size_t BlockAllocator::AlignBytesToUpper(size_t bytes)
{
    if (bytes % m_blockSize == 0)
        return bytes;

    bytes = (bytes - (bytes % m_blockSize));
    bytes += m_blockSize;
    return bytes;
}
