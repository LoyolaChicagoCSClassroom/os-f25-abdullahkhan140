.set MAGIC, 0x1BADB002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.extern main
.global _start

_start:
    mov $stack_top, %esp
    call main
    cli
hang:
    hlt
    jmp hang

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:
