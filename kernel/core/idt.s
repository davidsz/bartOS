global idt_load
global enable_interrupts
global disable_interrupts

; Produces a wrapper around interrupt handlers
%macro DEFINE_ISR 2
global %1
extern %2
%1:
    pushad
    call %2
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

; TODO: Define all with meaningful names
DEFINE_ISR int21h, int21h_handler
DEFINE_ISR no_interrupt, no_interrupt_handler

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
