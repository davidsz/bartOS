#ifndef CORE_IDT_H
#define CORE_IDT_H

#include "registers.h"

// Interrupt Descriptor Table

namespace core {

typedef void(* INTERRUPT_CALLBACK)(core::Registers *);

void setup_idt();
extern "C" void enable_interrupts();
extern "C" void disable_interrupts();
void register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK interrupt_callback);

}; // namespace core

#endif // CORE_IDT_H
