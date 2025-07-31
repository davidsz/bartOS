#ifndef OS_CONFIG_H
#define OS_CONFIG_H

// Offsets of the entries in the GDT
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

// On x86 protected mode, the IDT can have up to 256 entries
#define TOTAL_INTERRUPTS 256

#endif // OS_CONFIG_H
