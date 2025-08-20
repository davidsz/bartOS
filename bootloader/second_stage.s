; The second stage is starting from 0x7e00
; Usable space ends at 0x7ffff (480.5 KiB, 961 sectors)
[BITS 32]
global _start_second_stage       ; Make the entry point visible to the linker
extern second_stage_cpp

section .text
_start_second_stage:
    call second_stage_cpp

; Common functions of the bootloader stages
%include "bootloader/common.s"
