#include "gdt.h"

#include <stdint.h>

// The GdtPointer is used to indicate the place of the GDT in memory
struct GdtPointer
{
    uint16_t size;
    uint32_t address;
} __attribute__((packed));

// Each descriptor represents a segment in memory
struct SegmentDescriptor
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high;
} __attribute__((packed));

// GDT is basically an array of segment descriptors
const int s_gdtSize = 3;
SegmentDescriptor s_gdt[s_gdtSize];
GdtPointer s_gdtPointer;

/*
*  Loads the GDT and flushes segment registers
*  Defined in gdt_flush.s
*
*  @param GdtPointer filled with address and size
*/
extern "C" void flush_gdt(const GdtPointer *);

void add_segment_descriptor(uint8_t i, uint32_t base, uint32_t limit, uint8_t access)
{
    SegmentDescriptor desc;

    // Setup the descriptor base address
    desc.base_low = base & 0xFFFF;
    desc.base_middle = (base >> 16) & 0xFF;
    desc.base_high = (base >> 24) & 0xFF;

    // Setup the descriptor limits
    desc.limit_low = limit & 0xFFFF;
    desc.limit_high = (limit >> 16) & 0xF;

    // Flag bits: |G|DB|L|R|
    desc.flags = 0b1100;

    // Access bits: |P|DPL(2)|S|E|DC|RW|A|
    desc.access = access;

    s_gdt[i] = desc;
}

namespace core {

void setup_gdt()
{
    // The first descriptor in the GDT must not be used,
    // and a segment selector with index 0 is always invalid.
    // This supports error detection and other kernel features.
    add_segment_descriptor(0, 0, 0, 0);

    // Code segment
    add_segment_descriptor(1, 0, 0xFFFFFFFF, 0b10011010);

    // Data segment
    add_segment_descriptor(2, 0, 0xFFFFFFFF, 0b10010010);

    // Tell the system where GDT is and reload segment registers
    s_gdtPointer.size = sizeof(SegmentDescriptor) * s_gdtSize;
    s_gdtPointer.address = (uint32_t)s_gdt;
    flush_gdt(&s_gdtPointer);
}

}; // namespace core
