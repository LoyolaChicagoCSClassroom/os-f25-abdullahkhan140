#include <stdint.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_ADDRESS;
static uint16_t cursor_pos = 0; // keeps track of where we're printing

void putc(int c) {
    if (c == '\n') {
        // move to start of next line
        cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH;
    } else {
        // write char with (0x07)
        vga_buffer[cursor_pos++] = (0x07 << 8) | (uint8_t)c; 
    }
    // move to next ine if we are at end of screen
    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        cursor_pos = 0; 
    }
}

void printf(const char *str) {
    // just loop through and print each character
    while (*str) {
        putc(*str++);
    }
}

void kernel_main() {
    printf("Hello, ESP printf!\n");
    
    // raw pointer to video memory
    char *vidmem = (char *)0xB8000; 
    int offset = 0;
    
    // print alphabet on first 2 rows
    for (int row = 0; row < 2; row++) {         
        for (int col = 0; col < 26; col++) {    
            vidmem[offset] = 'A' + col;         // char byte
            vidmem[offset + 1] = 0x07;          // color byte
            offset += 2;                        // skip to next char
        }
        offset = (row + 1) * 160;               // jump to next row (160 bytes = 80 chars * 2 bytes each)
    }
}
