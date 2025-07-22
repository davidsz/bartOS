; Bootloader is loaded between 0x7c00-0x7DFF (= 512 bytes)
[ORG 0x7c00]
; Starts in real (16 bits) mode
[BITS 16]

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Reserve the first 36 bytes for the BIOS Parameter Block
; The first 3 bytes of this is reserved for jumpig over the non-executeable data
jmp short start  ; 2 bytes
nop              ; 1 byte
times 33 db 0    ; 33 bytes

start:
    ; Start with setting a default value to all segment registers to maintain full control
    jmp 0:step2  ; Perform a far jump to set the CS register
step2:
    cli ; critical section begin (clear interrupt flag)
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov es, ax
    mov sp, 0x7c00
    sti ; critical section ends (set interrupt flag)

.load_protected:
    ; Disable interrupts. The interrupt vector of real mode starts from 0x0000:0x0000,
    ; which is not a valid address in protected mode, so an incoming interrupt could cause
    ; a fault. We can enable them again after setting a new Interrupt Descriptor Table.
    cli

    ; Load Global Descriptor Table
    lgdt[gdt_addr]

    ; Set Protection Enable bit in CR0 (Control Register 0)
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Set the value of the Code Segment register to CODE_SEG
    ; (the offset of the code segment descriptor) by jumpig
    jmp CODE_SEG:start_protected

; Global Descriptor Table
gdt_start:
; It always starts with a null descriptor
gdt_null:
    dd 0x0
    dd 0x0

gdt_code:         ; offset 0x8; CS should point to this
    dw 0xffff     ; Segment limit first 0-15 bits
    dw 0          ; Base first 0-15 bits
    db 0          ; Base 16-23 bits
    db 10011010b  ; Access byte
    db 11001111b  ; High 4 bit flags and the low 4 bit flags
    db 0          ; Base 24-31 bits

gdt_data:         ; offset 0x10; DS, SS, ES, FS, GS
    dw 0xffff     ; Segment limit first 0-15 bits
    dw 0          ; Base first 0-15 bits
    db 0          ; Base 16-23 bits
    db 10010010b  ; Access byte
    db 11001111b  ; High 4 bit flags and the low 4 bit flags
    db 0          ; Base 24-31 bits

gdt_end:
; A struct that describes the place of GDT in memory
gdt_addr:
    dw gdt_end - gdt_start-1 ; size
    dd gdt_start             ; address

; 32 bits mode from this point
[BITS 32]
start_protected:
    ; Now we have a basic segmentation enabled, correct the values of the segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Enable the A20 line (to use more then 20 memory buses and access more then 1MB of memory)
    in al, 0x92       ; Read the value of System Control Port A
    or al, 00000010b  ; Set the corresponding bit to 1
    out 0x92, al      ; Write back the value of the port

    ; TODO: Load the kernel here

; Pad the rest of the space with zeros
times 510-($ - $$) db 0
; BIOS expects the last 2 bytes of the boot sector to be 0xAA55 (boot signature)
dw 0xAA55
