global idt_load
global enable_interrupts
global disable_interrupts

; Produces a wrapper around interrupt handlers
%macro DEFINE_ISR 2
global %1
extern %2
%1:
    cli
    pushad
    call %2
    popad
    sti
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
