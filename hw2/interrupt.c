#include <stdint.h>
#include "interrupt.h"
#include "keyboard.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void memset(char *dest, char val, unsigned int len) {
    for (unsigned int i = 0; i < len; i++)
        dest[i] = val;
}

struct idt_entry idt_entries[256];
struct idt_ptr   idt_ptr;
struct tss_entry tss_ent;

struct gdt_entry_bits gdt[] = {
    {0},
    {
        .limit_low = 0xFFFF, .base_low = 0,
        .accessed = 0, .read_write = 1, .conforming_expand_down = 0,
        .code = 1, .always_1 = 1, .DPL = 0, .present = 1,
        .limit_high = 0xF, .available = 0, .always_0 = 0,
        .big = 1, .gran = 1, .base_high = 0
    },
    {
        .limit_low = 0xFFFF, .base_low = 0,
        .accessed = 0, .read_write = 1, .conforming_expand_down = 0,
        .code = 0, .always_1 = 1, .DPL = 0, .present = 1,
        .limit_high = 0xF, .available = 0, .always_0 = 0,
        .big = 1, .gran = 1, .base_high = 0
    },
    {
        .limit_low = 0xFFFF, .base_low = 0,
        .accessed = 0, .read_write = 1, .conforming_expand_down = 0,
        .code = 1, .always_1 = 1, .DPL = 3, .present = 1,
        .limit_high = 0xF, .available = 0, .always_0 = 0,
        .big = 1, .gran = 1, .base_high = 0
    },
    {
        .limit_low = 0xFFFF, .base_low = 0,
        .accessed = 0, .read_write = 1, .conforming_expand_down = 0,
        .code = 0, .always_1 = 1, .DPL = 3, .present = 1,
        .limit_high = 0xF, .available = 0, .always_0 = 0,
        .big = 1, .gran = 1, .base_high = 0
    },
    {0}
};

struct seg_desc gdt_desc = {
    .sz = sizeof(gdt) - 1,
    .addr = (uint32_t)&gdt
};

static void tss_flush(uint16_t selector) {
    __asm__ __volatile__("ltr %0" : : "r"(selector));
}

static void write_tss(struct gdt_entry_bits *entry) {
    uint32_t base  = (uint32_t)&tss_ent;
    uint32_t limit = base + sizeof(struct tss_entry);

    entry->limit_low  = limit & 0xFFFF;
    entry->base_low   = base & 0xFFFFFF;
    entry->accessed   = 1;
    entry->read_write = 0;
    entry->code       = 1;
    entry->always_1   = 0;
    entry->DPL        = 3;
    entry->present    = 1;
    entry->limit_high = (limit >> 16) & 0xF;
    entry->big        = 0;
    entry->gran       = 0;
    entry->base_high  = (base >> 24) & 0xFF;

    memset((char*)&tss_ent, 0, sizeof(tss_ent));

    static uint8_t kernel_stack[4096] __attribute__((aligned(16)));

    tss_ent.ss0  = 0x10;
    tss_ent.esp0 = (uint32_t)(kernel_stack + sizeof(kernel_stack));
    tss_ent.cs   = 0x0B;
    tss_ent.ss = tss_ent.ds = tss_ent.es = tss_ent.fs = tss_ent.gs = 0x13;

    tss_flush(0x2B);
}

void load_gdt(void) {
    extern struct seg_desc gdt_desc;
    __asm__ __volatile__(
        "cli\n"
        "lgdt %[gdt_desc]\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%ss\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "ljmp $0x08, $.flush\n"
        ".flush:\n"
        :
        : [gdt_desc] "m"(gdt_desc)
        : "ax"
    );
}

void PIC_sendEOI(uint8_t irq) {
    if (irq >= 8)
        outb(PIC_2_COMMAND, PIC_EOI);
    outb(PIC_1_COMMAND, PIC_EOI);
}

void IRQ_set_mask(uint8_t irq_line) {
    uint16_t port = (irq_line < 8) ? PIC_1_DATA : PIC_2_DATA;
    if (irq_line >= 8) irq_line -= 8;
    uint8_t value = inb(port) | (1 << irq_line);
    outb(port, value);
}

void IRQ_clear_mask(uint8_t irq_line) {
    uint16_t port = (irq_line < 8) ? PIC_1_DATA : PIC_2_DATA;
    if (irq_line >= 8) irq_line -= 8;
    uint8_t value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}

void remap_pic(void) {
    outb(PIC_1_CTRL, 0x11);
    outb(PIC_2_CTRL, 0x11);
    outb(PIC_1_DATA, 0x20);
    outb(PIC_2_DATA, 0x28);
    outb(PIC_1_DATA, 0x04);
    outb(PIC_2_DATA, 0x02);
    outb(PIC_1_DATA, 0x01);
    outb(PIC_2_DATA, 0x01);
    outb(PIC_1_DATA, 0xFF);
    outb(PIC_2_DATA, 0xFF);
    IRQ_clear_mask(1);
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags;
}

static inline void idt_flush(struct idt_ptr *ptr) {
    __asm__ __volatile__("lidt (%0)" : : "r"(ptr));
}

__attribute__((interrupt))
void stub_isr(struct interrupt_frame *frame) {
    __asm__ __volatile__("cli; hlt");
}

__attribute__((interrupt))
void keyboard_handler(struct interrupt_frame *frame) {
    uint8_t scancode = inb(0x60);
    handle_keyboard_input(scancode);
    PIC_sendEOI(1);
}

void init_idt(void) {
    write_tss(&gdt[5]);
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;
    memset((char*)&idt_entries, 0, sizeof(idt_entries));
    for (int i = 0; i < 256; i++)
        idt_set_gate(i, (uint32_t)stub_isr, 0x08, 0x8E);
    idt_set_gate(0x21, (uint32_t)keyboard_handler, 0x08, 0x8E);
    idt_flush(&idt_ptr);
}
