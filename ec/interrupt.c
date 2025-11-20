#include "interrupt.h"
#include "rprintf.h"

/* I/O functions */
void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* TSS stub */
void write_tss(struct gdt_entry_bits *entry) {
    (void)entry; // stub: do nothing for now
}

/* Initialize PIC / interrupts (stub) */
void init_interrupts(void) {
    rprintf("Interrupts initialized.\n");
    // Normally here you would remap PIC, set IDT, etc.
}

/* ISR stubs */
void stub_isr(void) {
    // Empty: no floating-point instructions
}

void keyboard_handler(void) {
    // Empty: no floating-point instructions
}

