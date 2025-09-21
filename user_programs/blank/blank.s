[BITS 32]

section .asm

global _start

_start:
    ; Put two arguments to the stack
    push 20
    push 30
    ; Calling kernel command 0 (sum) through the interrupt 0x80
    mov eax, 0
    int 0x80
    add esp, 8

    push message
    ; Calling kernel command 1 (print)
    mov eax, 1
    int 0x80
    add esp, 4

    jmp $

section .data
message: db '> I can talk with the kernel!', 0
