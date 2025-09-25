#include <stdint.h>
#include "rprintf.h"  // esp_printf

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
#define MULTIBOOT2_HEADER_ARCHITECTURE  0
#define MULTIBOOT2_HEADER_LENGTH         24
#define MULTIBOOT2_HEADER_CHECKSUM      \
    (-(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH))

__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    MULTIBOOT2_HEADER_MAGIC,
    MULTIBOOT2_HEADER_ARCHITECTURE,
    MULTIBOOT2_HEADER_LENGTH,
    MULTIBOOT2_HEADER_CHECKSUM,
    0, 0, 8
};

// VGA text memory
unsigned short * const vram = (unsigned short *)0xB8000;
const unsigned char color = 7;
int cursor_pos = 0;

// The putc function the terminal driver (and esp_printf) will use
int putc(int c) {
    if (c == '\n') {
        cursor_pos += 80 - (cursor_pos % 80);
    } else {
        vram[cursor_pos++] = (color << 8) | (c & 0xFF);
    }

    if (cursor_pos >= 80 * 25) {
        // Scroll up one line
        for (int i = 0; i < 80 * 24; i++) {
            vram[i] = vram[i + 80];
        }
        // Clear last line
        for (int i = 80 * 24; i < 80 * 25; i++) {
            vram[i] = (color << 8) | ' ';
        }
        cursor_pos -= 80;
    }
    return c;
}

void kernel_main() {
    esp_printf(putc, "Hello from kernel!\n");
    esp_printf(putc, "Testing scroll:\n");

    for (int i = 0; i < 40; i++) {
        esp_printf(putc, "Line %d\n", i);
    }

    // Keep kernel running
    while (1) {}
}
