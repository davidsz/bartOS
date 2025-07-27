ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M;

    .multiboot :
    {
        *(.multiboot)
    }

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        start_ctors = .;
        KEEP(*( .ctors ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .ctors.* )));
        end_ctors = .;
		*(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}
