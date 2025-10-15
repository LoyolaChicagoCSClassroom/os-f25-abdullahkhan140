#include <stdint.h>
#include "interrupt.h"
#include "rprintf.h"   // include our adapted printf

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_ADDRESS;
static uint16_t cursor_pos = 0; // keeps track of where we're printing

// Basic VGA output
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

// Simple string print
void printf(const char *str) {
    while (*str) putc(*str++);
}

// Keyboard map (from elsewhere)
extern unsigned char keyboard_map[128];

// Kernel entry point
void kernel_main() {
    // Initialize interrupts
    remap_pic();
    load_gdt();
    init_idt();

    // Unmask keyboard IRQ1
    IRQ_clear_mask(1);

    // Enable interrupts
    asm("sti");

    // Print startup messages
    esp_printf((func_ptr)putc, "Keyboard Driver Initialized\n");
    esp_printf((func_ptr)putc, "Start typing...\n\n");

    // Demonstrate esp_printf with numbers
    esp_printf((func_ptr)putc, "Hello OS! Number: %d Hex: 0x%x\n", 123, 0x7B);

    // Main loop: halt CPU until next interrupt
    while (1) {
        asm("hlt");
    }
}
