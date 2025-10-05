[BITS 32]

global putchar:function
global puts:function
global getkey:function

; int putchar(int c)
putchar:
    push ebp
    mov ebp, esp
    push dword [ebp+8]
    mov eax, 3
    int 0x80
    add esp, 4
    pop ebp
    ret

; int puts(const char *message)
puts:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1
    int 0x80
    mov eax, 0
    add esp, 4
    pop ebp
    ret

; int getkey()
getkey:
    push ebp
    mov ebp, esp
    mov eax, 2
    int 0x80
    pop ebp
    ret
