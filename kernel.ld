ENTRY(kernel_main)
SECTIONS
{
    . = 0x100000;

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .bss : {
        *(.bss)
    }
}
