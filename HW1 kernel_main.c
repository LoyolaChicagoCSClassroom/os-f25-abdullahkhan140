#include <stdint.h>
#include "rprintf.h"

#define MB2_MAGIC       0xE85250D6
#define VGA_ADDRESS     0xB8000
#define SCREEN_ROWS     25
#define SCREEN_COLS     80

// current position on screen
static int screen_pos = 0;

// Write one character to VGA text buffer
void vga_write(int ch) {
    volatile char *vram = (char *)VGA_ADDRESS;

    if (ch == '\n') {
        // jump to next line
        screen_pos += SCREEN_COLS - (screen_pos % SCREEN_COLS);
    } else {
        vram[screen_pos * 2] = (char)ch;   // character byte
        vram[screen_pos * 2 + 1] = 0x07;   // attribute byte (gray on black)
        screen_pos++;
    }

    // wrap around when reaching end of screen (temporary before scroll)
    if (screen_pos >= SCREEN_ROWS * SCREEN_COLS) {
        screen_pos = 0;
    }
}

// read a byte from I/O port
uint8_t port_read(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

// entry point for the kernel
void kernel_main(void) {
    // print test message
    esp_printf(vga_write, "Hello from my OS!\r\n");

    // simple polling loop for keyboard input
    for (;;) {
        uint8_t state = port_read(0x64);
        if (state & 1) {
            uint8_t sc = port_read(0x60);
            // TODO: convert scancode to ASCII and print using vga_write()
        }
    }
}
