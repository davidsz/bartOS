[BITS 32]

global exec:function
global get_process_arguments:function

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

; void get_process_arguments(process_arguments *arguments);
get_process_arguments:
    push ebp
    mov ebp, esp
    mov eax, 7
    push dword [ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret
