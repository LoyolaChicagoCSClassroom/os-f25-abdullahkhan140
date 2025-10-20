#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

#define PIC_1_CTRL 0x20
#define PIC_1_DATA 0x21
#define PIC_2_CTRL 0xA0
#define PIC_2_DATA 0xA1
#define PIC_EOI    0x20

struct gdt_entry_bits {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  accessed       : 1;
    uint8_t  read_write     : 1;
    uint8_t  conforming_expand_down : 1;
    uint8_t  code           : 1;
    uint8_t  always_1       : 1;
    uint8_t  DPL            : 2;
    uint8_t  present        : 1;
    uint8_t  limit_high     : 4;
    uint8_t  available      : 1;
    uint8_t  always_0       : 1;
    uint8_t  big            : 1;
    uint8_t  gran           : 1;
    uint8_t  base_high;
} __attribute__((packed));

struct seg_desc {
    uint16_t sz;
    uint32_t addr;
} __attribute__((packed));

struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap_base;
} __attribute__((packed));

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct interrupt_frame {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void load_gdt(void);
void write_tss(struct gdt_entry_bits *entry);
void remap_pic(void);
void PIC_sendEOI(uint8_t irq);
void IRQ_set_mask(uint8_t irq_line);
void IRQ_clear_mask(uint8_t irq_line);
void init_idt(void);
__attribute__((interrupt)) void keyboard_handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void stub_isr(struct interrupt_frame* frame);

#endif
