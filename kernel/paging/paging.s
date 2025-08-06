[BITS 32]
global paging_load_directory
global paging_enable

section .text

; void paging_load_directory(uint32_t *directory);
; Moves the directory pointer to CR3.
; stack layout (cdecl):
;   [esp+4]  = directory (IdtPointer *)
paging_load_directory:
    mov eax, [esp+4]
    mov cr3, eax
    ret

; void paging_enable();
; Set the first bit of CR0 to enable paging.
paging_enable:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret
