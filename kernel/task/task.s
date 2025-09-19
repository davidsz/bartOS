[BITS 32]

global restore_task
global restore_general_purpose_registers
global change_data_segment

section .text

; void restore_task(Registers *reg);
restore_task:
    mov ebp, esp
    mov ebx, [ebp+4]
    push dword [ebx+44]     ; Push the data/stack selector
    push dword [ebx+40]     ; Push the stack pointer

    pushf                   ; Push the flags
    pop eax
    or eax, 0x200           ; Set the Interrupt Flag to enable interrupts
    push eax

    push dword [ebx+32]     ; Push the code segment
    push dword [ebx+28]     ; Push the IP to execute

    ; Setup segment registers
    ; TODO: Use change_data_segment() here
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebx+4]
    call restore_general_purpose_registers
    add esp, 4

    iretd    ; Leave kernel land and execute in user land

; void restore_general_purpose_registers(Registers *reg);
restore_general_purpose_registers:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop esp
    ret

; void change_data_segment(uint16_t selector);
change_data_segment:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop ebp
    ret
