#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_ADDRESS;
static uint16_t cursor_pos = 0; 

void putc(char c) {
    if (c == '\n') {
        cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH;
    } else {
        vga_buffer[cursor_pos] = (0x07 << 8) | (uint8_t)c; 
        cursor_pos++;
    }
    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        cursor_pos = 0;
    }
}
