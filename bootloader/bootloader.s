; Bootloader is loaded between 0x7c00-0x7DFF (= 512 bytes)
[ORG 0x7c00]
; Starts in real (16 bits) mode
[BITS 16]

KERNEL_LOAD_ADDR    equ 0x00100000     ; Load the kernel to this address
ELF_HEADER_ADDR     equ 0x00080000     ; Load the first sector of the ELF file here
SECTOR_SIZE         equ 512            ; bytes
KERNEL_DISC_OFFSET  equ SECTOR_SIZE    ; The kernel starts from the second sector

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


; Reserve the first 36 bytes for the BIOS Parameter Block
; The first 3 bytes of this is reserved for jumpig over the non-executeable data
jmp short step1  ; 2 bytes
nop              ; 1 byte
times 33 db 0    ; 33 bytes

step1:
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
    ; (the offset of the code segment descriptor) by far jumpig
    jmp CODE_SEG:start_protected

; Global Descriptor Table
gdt_start:
; It always starts with a null descriptor
gdt_null:
    dd 0x0
    dd 0x0

; The base 0 and a limit 0xFFFFFFFF is stated in the multiboot specs
gdt_code:         ; offset 0x8; CS should point to this
    dw 0xffff     ; Segment limit first 0-15 bits
    dw 0          ; Base first 0-15 bits
    db 0          ; Base 16-23 bits
    db 10011010b  ; Access byte
    db 11001111b  ; High 4 bit flags and the low 4 bit flags
    db 0          ; Base 24-31 bits

; The base 0 and a limit 0xFFFFFFFF is stated in the multiboot specs
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

    ; Enable the A20 line (to use more than 20 memory buses and access more than 1MB of memory)
    in al, 0x92       ; Read the value of System Control Port A
    or al, 00000010b  ; Set the corresponding bit to 1
    out 0x92, al      ; Write back the value to the port

    ; Load the kernel from disk to memory
    ; First we read the ELF header, only one sector
    mov eax, 1               ; LBA (Logical Block Address); the kernel starts from the second sector
    mov ecx, 1               ; Number of sectors to read
    mov edi, ELF_HEADER_ADDR ; Destination address (at 1Mb)
    call ata_lba_read        ; Call the routine (EAX, ECX, EDI)
    ; We verify that the loaded sector is an ELF header
    mov eax, dword [edi]  ; Let's look into the content of the loaded sector
    cmp eax, 0x464C457F ; ELF magic constant: 'F', 'L', 'E', 0x7F
    jne kernel_not_elf

    mov esi, edi       ; ELF header address
    mov ebx, dword [esi + 0x1C]   ; e_phoff (program header offset)
    add ebx, ELF_HEADER_ADDR  ; Absolute address of the first program header
    movzx edx, word [esi + 0x2A] ; e_phentsize (program header size)
    movzx ecx, word [esi + 0x2C]  ; e_phnum (number of program headers/segments)

.next_ph:
    mov eax, [ebx + 0x00]                ; p_type is at the beginning of PH
    cmp eax, 1              ; 1 = PT_LOAD; we have to load the segment
    jne .skip_ph
    call load_segment                    ; Load the segment (IN: EBX)
.skip_ph:
    add ebx, edx     ; Next PH = PH address + PH size
    loop .next_ph    ; Decrease ECX and repeat

    mov eax, dword [ELF_HEADER_ADDR + 0x18] ; e_entry
    jmp eax      ; Call the entry point of the loaded ELF

kernel_not_elf:
    mov eax, 1                ; LBA (Logical Block Address); the kernel starts from the second sector
    mov ecx, 100              ; Number of sectors to read
    mov edi, KERNEL_LOAD_ADDR ; Destination address (at 1Mb)
    call ata_lba_read         ; Call the routine (EAX, ECX, EDI)
    jmp CODE_SEG:KERNEL_LOAD_ADDR   ; Jump to the loaded kernel

; Read a segment from the ELF file into memory
; IN: ebx = Program header address
load_segment:
    pusha

    mov esi, ebx                ; Program header address
    mov eax, [esi + 0x04]       ; p_offset (file offset)
    mov ecx, [esi + 0x10]       ; p_filesz (Bytes to read)

    ; Calculate the number of sectors to read
    ; Number of sectors = (p_filesz + (SECTOR_SIZE - 1)) / SECTOR_SIZE
    push eax                     ; Backup p_offset
    push ecx                     ; Backup p_filesz
    xor edx, edx                         ; Null out EDX for division
    mov ebx, SECTOR_SIZE      ; Divisor
    add ecx, ebx              ; p_filesz + SECTOR_SIZE
    dec ecx                   ; p_filesz + SECTOR_SIZE - 1
    mov eax, ecx
    div ebx             ; EAX = EDX:EAX / EBX; EDX = remainder
    mov ebx, eax        ; Result: ebx = number of sectors to read
    pop ecx
    pop eax

    ; Read EBX sectors from EAX offset
    push eax                       ; Backup p_offset
    push ecx                       ; Backup p_filesz
    add eax, KERNEL_DISC_OFFSET    ; Absolute address on disc
    shr eax, 9                     ; Division by 512 -> Starting LBA
    mov ecx, ebx                   ; Number of sectors to read
    mov edi, [esi + 0x08]          ; p_vaddr (destination address)
    call ata_lba_read              ; Call the routine (EAX, ECX, EDI)
    pop ecx
    pop eax

    ; Padding with zero
    add edi, ecx                         ; Start zeroing here: p_vaddr + p_filesz
    mov edx, [esi + 0x14]                ; p_memsz (Memory size allocated for the segment)
    sub edx, ecx                         ; Amount of zeros: edx = p_memsz - p_filesz
    jbe .done
    xor eax, eax                         ; Null byte
.fill_zero:
    stosb                ; Writes AL to [EDI], then increments EDI
    dec edx
    jnz .fill_zero       ; Repeat until EDX is zero

.done:
    popa
    ret                  ; End of load_segment routine

; Using the ATA controller to read sectors from the hard drive, addressed by Logical Block Address
; IN: eax - Logical Block Address of the starting poin
;     ecx - Number of sectors to read
;     edi - Destination address to load
ata_lba_read:
    pusha
    mov ebx, eax ; Backup the LBA and restore it later in each step

    ; Device/Head register (port 0x1F6)
    ; 7   6   5   4   3   2   1   0
    ; 1   L   1   D   H   H   H   H
    ; -----------------------------
    ; 1 = Reserved bits
    ; L = LBA mode enabled
    ; D = Device (master is 0, slave is 1)
    ; H = highest 4 bits of the LBA
    ; -----------------------------
    shr eax, 24        ; Shift the highest 4 bits of the LBA into the lower 4 bits
    or eax, 11100000b  ; Select LBA mode and the master drive
    mov dx, 0x1F6      ; Specify the port for the Device/Head register
    out dx, al         ; Send the value to the port (AL is the lower 8 bits of EAX)

    ; Sector Count register (port 0x1F2)
    mov eax, ecx       ; We specified the number of sectors to read earlier in ECX
    mov dx, 0x1F2
    out dx, al

    ; LBA Low, bits 0-7 (port 0x1F3)
    mov eax, ebx       ; Restore the backup LBA
    mov dx, 0x1F3
    out dx, al         ; It's easy to send only the lowest 8 bits of EAX

    ; LBA Mid, bits 8-15 (port 0x1F4)
    mov eax, ebx       ; Restore the backup LBA
    mov dx, 0x1F4
    shr eax, 8         ; Bring the next 8 bits of the LBA into the lower 8 bits of EAX
    out dx, al

    ; LBA High, bits 16-23 (port 0x1F5)
    mov eax, ebx       ; Restore the backup LBA
    mov dx, 0x1F5
    shr eax, 16
    out dx, al

    ; Command/Status register (port 0x1F7)
    mov dx, 0x1f7
    mov al, 0x20       ; Reading (0x20) or writing (0x30) sectors
    out dx, al

.next_sector:            ; Reading a sector
    push ecx             ; ECX is the loop counter, but will have an other purpose, so save it on stack
.try_again:              ; Checking if we can read by polling in a loop
    mov dx, 0x1f7        ; Reading status from the Command/Status register
    in al, dx
    test al, 00001000b   ; Test the Data Request Ready (DRQ) bit
    jz .try_again

    ; We can read a sector from the Data register (port 0x1F0)
    mov ecx, 256         ; Number of words to read; 256 * 2 bytes = 512 bytes = 1 sector
    mov dx, 0x1F0
    rep insw             ; Read ECX number of words from the Data register into [ES:DI] and increases DI by 2
    pop ecx              ; Restore ECX from stack
    loop .next_sector    ; Decrease ECX and repeat

    popa
    ret                  ; End of the ata_lba_read routine

; Pad the rest of the space with zeros
times 510-($ - $$) db 0
; BIOS expects the last 2 bytes of the boot sector to be 0xAA55 (boot signature)
dw 0xAA55
