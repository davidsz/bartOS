global inb
global inw
global outb
global outw

section .text

; uint8_t inb(uint16_t port);
; stack layout (cdecl):
;   [esp+4]  = port (uint16_t)
; return value: eax
inb:
    xor eax, eax      ; Clear the eax register
    mov dx, [esp+4]   ; Address of the port
    in al, dx         ; Read the byte from the port
    ret

; uint16_t inw(uint16_t port);
; stack layout (cdecl):
;   [esp+4]  = port (uint16_t)
; return value: eax
inw:
    xor eax, eax      ; Clear the eax register
    mov dx, [esp+4]   ; Address of the port
    in ax, dx         ; Read the word from the port
    ret

; void outb(uint16_t port, uint8_t data);
; stack layout (cdecl):
;   [esp+4]  = port (uint16_t)
;   [esp+8]  = value (uint8_t char)
outb:
    mov al, [esp+8]   ; The byte to send
    mov dx, [esp+4]   ; Address of the port
    out dx, al        ; Send the byte to the port
    ret

; void outw(uint16_t port, uint16_t data);
; stack layout (cdecl):
;   [esp+4]  = port (uint16_t)
;   [esp+8]  = value (uint16_t)
outw:
    mov ax, [esp+8]   ; The word to send
    mov dx, [esp+4]   ; Address of the port
    out dx, ax        ; Send the word to the port
    ret
