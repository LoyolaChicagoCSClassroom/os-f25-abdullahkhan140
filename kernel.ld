ENTRY(kernel_main)
OUTPUT_FORMAT(elf32-i386)
SECTIONS
{
  . = 0x100000;  /* load address */

  .text : {
    *(.text)
  }

  .rodata : {
    *(.rodata)
  }

  .data : {
    *(.data)
  }

  .bss : {
    *(.bss)
    *(COMMON)
  }
}
