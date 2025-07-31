#ifndef CORE_IDT_H
#define CORE_IDT_H

// Interrupt Descriptor Table

namespace core {

void setup_idt();
extern "C" void enable_interrupts();
extern "C" void disable_interrupts();

}; // namespace core

#endif // CORE_IDT_H
