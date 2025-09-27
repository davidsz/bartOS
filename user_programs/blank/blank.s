[BITS 32]

section .asm

global _start

_start:
    push message
    ; Calling kernel command 1 (print)
    mov eax, 1
    int 0x80
    add esp, 4

_input_output_loop:
    call getkey
    push eax     ; The returned char is the argument of putchar
    mov eax, 3   ; Kernel command 3 (putchar)
    int 0x80
    add esp, 4
    jmp _input_output_loop

getkey:
    ; Calling kernel command 2 (getkey)
    mov eax, 2
    int 0x80
    cmp eax, 0x00
    je getkey
    ret

section .data
message: db '> Your message: ', 0
