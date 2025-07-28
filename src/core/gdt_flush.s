; void flush_gdt(GdtPointer *gdt_ptr);
; stack layout (cdecl):
;   [esp+4]  = gdt_ptr (GdtPointer *)
global flush_gdt

section .text
flush_gdt:
    mov eax, [esp+4]
    lgdt [eax]            ; load GDTR

    mov ax, 0x10          ; 0x10 = data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Perform a far jump to flush the CS register
    jmp 0x08:flush_cs     ; (0x08) = code segment selector
flush_cs:
    ret
