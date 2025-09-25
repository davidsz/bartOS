global idt_load
global enable_interrupts
global disable_interrupts
global interrupt_pointer_table

; A general interrupt handler function which will be called by each interrupt
; if not overridden by a specialized handler
extern interrupt_handler
%macro interrupt 1
global int%1
int%1:
    ; We are composing a structure with the processor state (core::Registers interrupt_frame)
    ; IP, flags and segment registers are already pushed by the processor
    pushad         ; Push the general purpose registers to the stack
    push esp       ; Push the stack pointer which points to the interrupt frame
    push dword %1  ; Push the interrupt number
    call interrupt_handler
    add esp, 8
    popad
    iret
%endmacro

section .text

; void idt_load(IdtPointer *idt_ptr);
; stack layout (cdecl):
;   [esp+4]  = idt_ptr (IdtPointer *)
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret

; void enable_interrupts();
enable_interrupts:
    sti
    ret

; void disable_interrupts();
disable_interrupts:
    cli
    ret

; Create a handler for each interrupt to call the general interrupt_handler
%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

; A specialized interrupt handler for the 0x80 interrupt
global int80h
extern int80h_handler
int80h:
    ; We are composing a structure with the processor state (core::Registers interrupt_frame)
    ; IP, flags and segment registers are already pushed by the processor
    pushad      ; Push the general purpose registers to the stack

    push esp    ; Push the stack pointer which points to the interrupt frame
    push eax    ; EAX holds the command; push it to the stack for int80h_handler
    call int80h_handler    ; Now its arguments are the command and the processor state structure
    mov dword[tmp_res], eax
    add esp, 8

    popad       ; Restore general purpose registers for user land
    mov eax, [tmp_res]
    iretd

section .data
tmp_res: dd 0    ; We store the return result from int80h here

%macro interrupt_array_entry 1
    dd int%1
%endmacro

; A pointer table that contains the addresses of all interrupt handlers
interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep
