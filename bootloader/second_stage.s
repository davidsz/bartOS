; The second stage is starting from 0x7e00
; Usable space ends at 0x7ffff (480.5 KiB, 961 sectors)
[BITS 32]
global _start_second_stage       ; Make the entry point visible to the linker
extern second_stage_cpp          ; Implemented in C++

section .bss
; TODO:
; - Collect boot information
; - https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
multiboot_info:
    resb 96

section .text
_start_second_stage:
    call second_stage_cpp
    mov ecx, eax

    ; Fill EAX and EBX to comply with the Multiboot standard
    mov eax, 0x2BADB002
    mov ebx, multiboot_info
    call ecx

; Common functions of the bootloader stages
%include "bootloader/common.s"
