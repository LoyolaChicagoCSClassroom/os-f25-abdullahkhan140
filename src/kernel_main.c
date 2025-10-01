#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_ADDRESS;
static uint16_t cursor_pos = 0;

void putc(int c) {
    if (c == '\n') {
        cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH;
    } else {
        vga_buffer[cursor_pos++] = (0x07 << 8) | (uint8_t)c; 
    }

    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        cursor_pos = 0; 
    }
}

void printf(const char *str) {
    while (*str) {
        putc(*str++);
    }
}

void kernel_main() {
    printf("Hello, ESP printf!\n");

    char *vidmem = (char *)0xB8000; 
    int offset = 0;

    for (int row = 0; row < 2; row++) {         
        for (int col = 0; col < 26; col++) {    
            vidmem[offset] = 'A' + col;         
            vidmem[offset + 1] = 0x07;          
            offset += 2;                        
        }
        offset = (row + 1) * 160;              
    }
}
