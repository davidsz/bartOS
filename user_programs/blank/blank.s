[BITS 32]

section .asm

global _start

_start:
    ; Calling kernel command 0 through the interrupt 0x80
    mov eax, 0
    int 0x80

    jmp $
