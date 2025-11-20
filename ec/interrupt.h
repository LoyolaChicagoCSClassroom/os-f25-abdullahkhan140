#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

/* PIC ports */
#define PIC_1_COMMAND 0x20
#define PIC_1_DATA    0x21
#define PIC_2_COMMAND 0xA0
#define PIC_2_DATA    0xA1
#define PIC_EOI       0x20

/* I/O functions */
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

/* TSS / GDT helper */
struct gdt_entry_bits;
void write_tss(struct gdt_entry_bits *entry);

/* Interrupt initialization */
void init_interrupts(void);

/* ISR stubs */
void stub_isr(void);
void keyboard_handler(void);

#endif

