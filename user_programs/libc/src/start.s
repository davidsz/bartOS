[BITS 32]

global _start
extern c_start

section .asm

_start:
    call c_start

    ; Call the exit command
    mov eax, 6
    int 0x80

    ret
