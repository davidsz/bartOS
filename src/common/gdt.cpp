#include "gdt.h"


// The GdtPointer is used to indicate the place of the GDT in memory
struct GdtPointer
{
    unsigned short size;
    unsigned int address;
} __attribute__ ((packed));

// Each descriptor represents a segment in memory
struct SegmentDescriptor
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char limit_high : 4;
    unsigned char flags : 4;
    unsigned char base_high;
} __attribute__ ((packed));

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

void add_segment_descriptor(unsigned short i, unsigned long long base, unsigned long long limit, unsigned char access)
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

namespace common {

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
    s_gdtPointer.address = (unsigned int)s_gdt;
    flush_gdt(&s_gdtPointer);
}

}; // namespace common
