; The second stage is starting from 0x7e00
; Usable space ends at 0x7ffff (480.5 KiB, 961 sectors)
[BITS 32]
global _start_second_stage       ; Make the entry point visible to the linker
extern second_stage_cpp          ; Implemented in C++

section .text
_start_second_stage:
    call second_stage_cpp
    add esp, 8
    jmp eax
    ; push dword 10
    ; push dword 0
    ; call second_stage_cpp
    ; add esp, 8

; Common functions of the bootloader stages
%include "bootloader/common.s"

global jump_to_address
jump_to_address:
    mov eax, [esp+4]
    jmp eax
