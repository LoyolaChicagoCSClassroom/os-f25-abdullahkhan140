#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    0xe85250d6,     
    0,              
    24,           
    0x11ADAF12,     
     0, 0, 8
};

#include <stdint.h>
#include "rprintf.h"

#define VGA_BASE          0xB8000
#define VGA_ROWS          25
#define VGA_COLS          80
#define VGA_ATTR          0x07   // light gray on black

static int cursor = 0;

// Write a character directly to VGA text buffer
static void vga_putc(int ch) {
    volatile char *vga = (volatile char *)VGA_BASE;

    if (ch == '\n') {
        // Move cursor to next line start
        cursor += VGA_COLS - (cursor % VGA_COLS);
    } else {
        vga[cursor * 2]     = (char)ch;  // character
        vga[cursor * 2 + 1] = VGA_ATTR;  // color attribute
        cursor++;
    }

    // Wrap around (temporary until scroll implemented)
    if (cursor >= VGA_ROWS * VGA_COLS) {
        cursor = 0;
    }
}

// Read one byte from an I/O port
static inline uint8_t io_read(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "dN"(port));
    return value;
}

// Kernel entry point
void kernel_main(void) {
    esp_printf(vga_putc, "Hello from my simple OS!\n");

    // Basic keyboard polling loop
    for (;;) {
        uint8_t status = io_read(0x64);
        if (status & 1) {
            uint8_t scancode = io_read(0x60);
            // TODO: translate scancode -> ASCII and print
        }
    }
}
