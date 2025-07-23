/* Hopefully this file is only temporary. */

ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M;
    .text :
    {
        /* TODO: The custom bootloader should handle this
           (.multiboot) */
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
