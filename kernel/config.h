#ifndef OS_CONFIG_H
#define OS_CONFIG_H

// Offsets of the entries in the GDT
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
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

#endif // OS_CONFIG_H
