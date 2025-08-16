[BITS 32]
global _start           ; Make the entry point visible to the linker
extern kernel_main      ; Function is defined in kernel.cpp
extern kernel_load_end  ; Symbol defined in the linker script
extern bss_end          ; Symbol defined in the linker script


; Declare constants for the multiboot header.
%define MAGIC     0x1BADB002            ; 'Magic number' lets bootloader find the header
%define ALIGN     (1 << 0)              ; Align loaded modules on page boundaries
;%define MEMINFO   (1 << 1)             ; Provide memory map
%define LOADADDR  (1 << 16)             ; The fields at offsets 12-28 in the Multiboot header are valid.
%define FLAGS     (ALIGN | LOADADDR)    ; This is the Multiboot 'flag' field
%define CHECKSUM  -(MAGIC + FLAGS)      ; Checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. The signature
; is in its own section so the header can be forced to be within the first 8 KiB.
section .multiboot
align 4
mb_header:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd mb_header        ; header_addr: The physical memory location at which the magic value is supposed to be loaded.
    dd 0x00100000       ; load_addr: Physical address of the beginning of the text segment.
    dd kernel_load_end  ; load_end_addr: End of the data segment.
                        ; Zero means the boot loader assumes that the text and data segments occupy the whole OS image file.
    dd bss_end          ; bss_end_addr: The physical address of the end of the bss segment.
                        ; If this field is zero, the boot loader assumes that no bss segment is present.
    dd _start           ; entry_addr: The physical address to which the boot loader should jump to start running the operating system.

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
