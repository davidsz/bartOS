[BITS 32]

global malloc:function
global free:function

; void *malloc(size_t size)
malloc:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 4
    int 0x80
    add esp, 4
    pop ebp
    ret

; void free(void *ptr)
free:
    push ebp
    mov ebp, esp
    mov eax, 5
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret
