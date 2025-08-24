; Common constants
FIRST_STAGE_ADDR    equ 0x00007C00     ; First stage is loaded between 0x7c00-0x7DFF (= 512 bytes)
SECOND_STAGE_ADDR   equ 0x00007E00     ; Second stage is starting after the first
SECTOR_SIZE         equ 512            ; Sector size in bytes
;-------------------------------------------------------------------------------

global ata_lba_read
; Using the ATA controller to read sectors from the hard drive, addressed by Logical Block Address
; Args on stack (cdecl):
;   [ebp+8]  = uint32 - LBA of the starting point
;   [ebp+12] = uint32 - Number of sectors to read
;   [ebp+16] = uint32 - Destination address to load
ata_lba_read:
    ; Prologue - create a stack frame
    push ebp
    mov ebp, esp
    push eax
    push ecx
    push edi
    ; Arguments
    mov eax, [ebp+8]    ; LBA
    mov ecx, [ebp+12]   ; Count (remaining sectors)
    mov edi, [ebp+16]   ; Destination pointer

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
    mov dx, 0x1F6      ; Specify the port as the Device/Head register
    out dx, al         ; Send the value to the port (AL is the lower 8 bits of EAX)

    ; Sector Count register (port 0x1F2)
    mov eax, ecx       ; We specified the number of sectors to read earlier in ECX
    mov dx, 0x1F2
    out dx, al

    ; LBA Low, bits 0-7 (port 0x1F3)
    mov eax, [ebp+8]   ; LBA
    mov dx, 0x1F3
    out dx, al         ; It's easy to send only the lowest 8 bits of EAX

    ; LBA Mid, bits 8-15 (port 0x1F4)
    mov eax, [ebp+8]   ; LBA
    mov dx, 0x1F4
    shr eax, 8         ; Bring the next 8 bits of the LBA into the lower 8 bits of EAX
    out dx, al

    ; LBA High, bits 16-23 (port 0x1F5)
    mov eax, [ebp+8]   ; LBA
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
    rep insw             ; Read a word from the Data register into [ES:DI] and increases DI by 2
    pop ecx              ; Restore ECX from stack
    loop .next_sector    ; Decrease ECX and repeat

    ; Epilogue - restore the stack
    pop     edi
    pop     ecx
    pop     eax
    pop     ebp
    ret                  ; End of the ata_lba_read routine
;-------------------------------------------------------------------------------

global bytes_to_sectors
; Calculate the number of sectors to read
; Number of sectors = (bytes + (SECTOR_SIZE - 1)) / SECTOR_SIZE
; Args on stack (cdecl):
;   [esp+4]  = uint32 - Number of bytes
; Returns:
;   eax = Number of sectors
bytes_to_sectors:
    ; Arguments
    mov eax, [esp+4]             ; Number of bytes

    xor edx, edx                 ; Null out EDX for division
    add eax, SECTOR_SIZE - 1     ; bytes + SECTOR_SIZE - 1
    mov ebx, SECTOR_SIZE         ; Divisor
    div ebx                      ; EAX = EDX:EAX / EBX; EDX = remainder

    ret                          ; End of the bytes_to_sectors routine
; -----------------------------------------------------------------------------
