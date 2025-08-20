ENTRY(_start_second_stage)
OUTPUT_FORMAT(binary)
SECTIONS
{
  . = 0x7E00;

  __image_start = .;

  .header :
  {
    LONG(__padded_end - __image_start)
    LONG(_start_second_stage)
  }

  .text : { *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss  : { *(.bss*) *(COMMON) }

  __padded_end = .;
}
