#include "idt.h"

#include "config.h"
#include "io.h"
#include "memory.h"
#include <stdint.h>

#include "console/console.h"

#define PIC1          0x20   // Master PIC
#define PIC2          0xA0   // Slave PIC
#define PIC1_COMMAND  0x20
#define PIC1_DATA     0x21
#define PIC2_COMMAND  0xA0
#define PIC2_DATA     0xA1

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

__attribute__((aligned(16))) InterruptDescriptor32 s_idt[TOTAL_INTERRUPTS];
IdtPointer s_idtPointer;

/*
*  Loads the IDT
*  Defined in idt.s
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

void remap_pics()
{
    core::outb(PIC1_COMMAND, 0x11); // Init
    core::outb(PIC2_COMMAND, 0x11);

    core::outb(PIC1_DATA, 0x20);    // Interrupt 0x20 (32) is where the ISRs start
    core::outb(PIC2_DATA, 0x28);    // Start from 0x28 (40)

    core::outb(PIC1_DATA, 0x04);    // Tell Master PIC about Slave PIC at IRQ2
    core::outb(PIC2_DATA, 0x02);    // Tell Slave PIC its cascade identity

    core::outb(PIC1_DATA, 0x01);    // 8086 mode
    core::outb(PIC2_DATA, 0x01);

    core::outb(PIC1_DATA, 0x0);     // Restore original masks
    core::outb(PIC2_DATA, 0x0);
}

namespace core {

// TODO: Don't leave these here
void idt_zero() { }

extern "C" void int21h();
extern "C" void int21h_handler()
{
    console::print("Keyboard pressed!\n");
    core::outb(0x20, 0x20); // Tell the PIC that the interrupt is handled
}

extern "C" void no_interrupt();
extern "C" void no_interrupt_handler()
{
    core::outb(0x20, 0x20); // Tell the PIC that the interrupt is handled
}

void setup_idt()
{
    // The IDT can have up to 256 entries.
    // 0-31:    Reserved for CPU exceptions
    // 8-15:    IRQs from PIC1 -> We map them to 32-39 to eliminate collisions
    // 112-119: IRQs from PIC2 -> We map them to 40-47 for the sake of order
    remap_pics();

    // Null the whole IDT out and fill with empty handlers
    memset(s_idt, 0, sizeof(s_idt));
    for (int i = 0; i < TOTAL_INTERRUPTS; i++)
        add_interrupt_descriptor(i, (void *)no_interrupt);

    // Add an entry for example to handle divide by zero
    add_interrupt_descriptor(0, (void *)idt_zero);
    add_interrupt_descriptor(0x21, (void *)int21h);

    // Tell the system where IDT is
    s_idtPointer.size = sizeof(s_idt) - 1; // The -1 comes from the IDT specs
    s_idtPointer.address = (uint32_t)s_idt;
    idt_load(&s_idtPointer);
}

}; // namespace core
