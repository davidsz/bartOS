.globl flush_gdt
.type flush_gdt, @function

flush_gdt:
    mov 4(%esp), %eax
    lgdt (%eax)                 # Load GDT

    movw $0x10, %ax             # 0x10: offset of data segment selector
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ljmp $0x08, $flush_cs       # Far jump to set CS
flush_cs:
    ret
