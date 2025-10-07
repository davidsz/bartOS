[BITS 32]

global exec:function

; void exec(char *command)
exec:
    push ebp
    mov ebp, esp
    mov eax, 0
    push dword [ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret
