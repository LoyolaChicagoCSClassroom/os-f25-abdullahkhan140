#include <stdint.h>
#include "interrupt.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_ADDRESS;
static uint16_t cursor_pos = 0; // keeps track of where we're printing

void putc(int c) {
    if (c == '\n') {
        cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH;
    } else {
        vga_buffer[cursor_pos++] = (0x07 << 8) | (uint8_t)c;
    }
    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        cursor_pos = 0; // wrap around
    }
}

void printf(const char *str) {
    while (*str) {
        putc(*str++);
    }
}

extern unsigned char keyboard_map[128];
extern void putc(int c);

void kernel_main() {
    // Initialize interrupts
    remap_pic();
    load_gdt();
    init_idt();

    // Unmask keyboard IRQ1
    IRQ_clear_mask(1);

    // Print startup messages
    printf("Keyboard Driver Initialized\n");
    printf("Start typing...\n\n");

    // Enable interrupts
    asm("sti");

    // Main loop: halt CPU until next interrupt
    while (1) {
        asm("hlt");
    }
}
