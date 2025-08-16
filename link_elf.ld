/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 1M;

	/* First put the multiboot header, as it is required to be put very early
	   in the image or the bootloader won't recognize the file format. */
	.multiboot BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
	}

	/* Next we'll put the .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
        /* The .ctors segment contains the constructors of global objects,
           which have to be called at the beginning of the kernel program.
           We make it available in C++ with this section. */
        start_ctors = .;
        KEEP(*( .ctors ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .ctors.* )));
        end_ctors = .;
        *(.data)
        kernel_load_end = .;
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		bss_end = .;
	}
}
