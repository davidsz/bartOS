[BITS 32]

global print:function

; void print(const char *message)
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1
    int 0x80
    pop ebp
    ret
