[BITS 32]
global _start

section .asm

_start:
    call print_message
_label:
    jmp _label

print_message:
    push ebp
    mov ebp, esp
    push message
    ; Calling kernel command 1 (print)
    mov eax, 1
    int 0x80
    add esp, 4
    pop ebp
    ret

section .data
message: db '!!! Message from the assembly !!!', 0
