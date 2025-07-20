global _start           ; Make the entry point visible to the linker
extern kernel_main      ; Function is defined elsewhere

; Declare constants for the multiboot header.
%define ALIGN     (1 << 0)              ; Align loaded modules on page boundaries
%define MEMINFO   (1 << 1)              ; Provide memory map
%define FLAGS     (ALIGN | MEMINFO)     ; This is the Multiboot 'flag' field
%define MAGIC     0x1BADB002            ; 'Magic number' lets bootloader find the header
%define CHECKSUM  -(MAGIC + FLAGS)      ; Checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. The signature
; is in its own section so the header can be forced to be within the first 8 KiB.
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; The stack grows downwards on x86. We create a 16 KiB stack and ensure
; 16-byte alignment as required by the System V ABI.
section .bss
align 16
stack_bottom:
    resb 16384        ; Reserve 16 KiB
stack_top:

; Kernel entry point
section .text
_start:
    ; Set up a stack before calling functions
    mov esp, stack_top

    ; Enter the high-level kernel
    call kernel_main
