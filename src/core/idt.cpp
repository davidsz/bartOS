#include "idt.h"

#include "config.h"
#include "memory.h"
#include <stdint.h>


// The IdtPointer is used to indicate the place of the IDT in memory
struct IdtPointer
{
    uint16_t size;
    uint32_t address;
} __attribute__((packed));

struct InterruptDescriptor32
{
    uint16_t offset_low;    // Offset bits 0 - 15
    uint16_t selector;      // GDT or LDT segment selector
    uint8_t zero;           // Unused, always zero
    uint8_t gate_type : 4;  // Gate type
    uint8_t flags : 4;      // S, DPL and P fields
    uint16_t offset_high;   // Offset bits 16 - 31
} __attribute__((packed));

InterruptDescriptor32 s_idt[TOTAL_INTERRUPTS];
IdtPointer s_idtPointer;

/*
*  Loads the IDT
*  Defined in idt_load.s
*
*  @param IdtPointer filled with address and size
*/
extern "C" void idt_load(struct IdtPointer *);

void add_interrupt_descriptor(uint8_t i, void *address)
{
    InterruptDescriptor32 desc;

    // Split the address into two 16 bits parts
    desc.offset_low = (uint32_t) address & 0x0000ffff;
    desc.offset_high = (uint32_t) address >> 16;

    desc.selector = KERNEL_CODE_SELECTOR;
    desc.zero = 0;

    // 0b0101: Task Gate (offset value is unused and should be set to zero)
    // 0b0110: 16-bit Interrupt Gate
    // 0b0111: 16-bit Trap Gate
    // 0b1110: 32-bit Interrupt Gate
    // 0b1111: 32-bit Trap Gate
    desc.gate_type = 0b1110;

    // P[1]   : Present bit; 1 if the descriptor is valid
    // DPL[2] : Descriptor Privilege Level; ring 3 is 0b11
    // S[1]   : Storage segment; 0 for interrupts and trap gates
    desc.flags = 0b1110;

    s_idt[i] = desc;
}

namespace core {

// TODO: Don't leave this here
void idt_zero() { }

void setup_idt()
{
    // Null the whole IDT out
    memset(s_idt, 0, sizeof(s_idt));

    add_interrupt_descriptor(0, (void *)idt_zero);

    // Tell the system where IDT is
    s_idtPointer.size = sizeof(s_idt) - 1; // The -1 comes from the IDT specs
    s_idtPointer.address = (uint32_t)s_idt;
    idt_load(&s_idtPointer);
}

}; // namespace core
