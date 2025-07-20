; void outb(unsigned short port, unsigned char data);
; stack layout (cdecl):
;   [esp+4]  = port (unsigned short)
;   [esp+8]  = value (unsigned char)
global outb

section .text
outb:
    mov al, [esp+8]   ; A küldendő byte -> AL
    mov dx, [esp+4]   ; I/O port címe -> DX
    out dx, al        ; Küldjük a byte-ot a megadott port-ra
    ret
