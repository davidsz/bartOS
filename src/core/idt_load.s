; void idt_load(IdtPointer *idt_ptr);
; stack layout (cdecl):
;   [esp+4]  = idt_ptr (IdtPointer *)
global idt_load

section .text
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret
