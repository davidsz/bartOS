#ifndef OS_CONFIG_H
#define OS_CONFIG_H

// Offsets of the entries in the GDT
// These offsets are always a multiple of 8 bytes. In binary, their lower
// three bits would be zero, but they can store a flags here.
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
// The selector is 0x18, + 0b11 Required Privilege Level flags -> Ring 3
#define USER_CODE_SELECTOR 0x1B
// The selector is 0x20, + 0b11 Required Privilege Level flags -> Ring 3
#define USER_DATA_SELECTOR 0x23
#define TASK_STATE_SELECTOR 0x28

// On x86 protected mode, the IDT can have up to 256 entries
#define TOTAL_INTERRUPTS 256

// Maximum number of global destructors
#define MAX_DESTRUCTORS 128

// 100MB heap size
#define HEAP_SIZE_BYTES 104857600
#define HEAP_BLOCK_SIZE 4096
#define HEAP_ADDRESS 0x01000000

// The most frequently used sector size
#define SECTOR_SIZE 512

// User land
#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END PROGRAM_VIRTUAL_ADDRESS_START - PROGRAM_STACK_SIZE

#endif // OS_CONFIG_H
