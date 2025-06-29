// Declare constants for the multiboot header.
.set ALIGN,    1<<0               // Align loaded modules on page boundaries
.set MEMINFO,  1<<1               // Provide memory map
.set FLAGS,    ALIGN | MEMINFO    // This is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002         // 'Magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)   // Checksum of above, to prove we are multiboot

/*
Declare a multiboot header that marks the program as a kernel. The signature is in
its own section so the header can be forced to be within the first 8 KiB of the
kernel file.
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
The stack grows downwards on x86. The stack is in its own section so it can be
marked nobits, which means the kernel file is smaller because it does not contain
an uninitialized stack. The stack on x86 must be 16-byte aligned according to the
System V ABI standard and de-facto extensions. The compiler will assume the
stack is properly aligned and failure to align the stack will result in
undefined behavior.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded.
*/
.section .text
.globl _start
.type _start, @function
_start:
	// Set up a stack before calling functions
	mov $stack_top, %esp

	// Do everything necessary before stepping into kernel_main in C++
	call call_constructors

	// Enter the high-level kernel.
	call kernel_main

    /*
    If the system has nothing more to do, put the computer into an
    infinite loop. To do that:
    1) Disable interrupts with cli
    2) Wait for the next interrupt to arrive with hlt
       Since they are disabled, this will lock up the computer.
    3) Jump to the hlt instruction if it ever wakes up
    */
	cli
1:	hlt
	jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging (readelf, objdump, etc.)
*/
.size _start, . - _start
