ENTRY(kernel_main)

SECTIONS
{
    .multiboot 0x100000 : { KEEP(*(.multiboot)) }

    .text 0x100010 : { *(.text) }

    .data : { *(.data) }
    .bss  : { *(.bss) *(COMMON) }
}
