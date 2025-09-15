[BITS 32]
global flush_gdt
global load_tss

section .text

; void flush_gdt(GdtPointer *gdt_ptr);
; stack layout (cdecl):
;   [esp+4]  = gdt_ptr (GdtPointer *)
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

; void load_tss(int ts_segment);
; stack layout (cdecl):
;   [esp+8]  = ts_segment (int)
load_tss:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8]
    ltr ax                ; Load Task Register
    pop ebp
    ret
