#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6

__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    0xe85250d6,     
    0,              
    24,            
    0x11ADAF12,     

     0, 0, 8
};
    
};

// Read a byte from a port
uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(value) : "dN"(port));
    return value;
}

// VGA text buffer address
unsigned short *vram = (unsigned short*)0xb8000;

// Text color: light gray on black
const unsigned char color = 7;

// Cursor position
int cursor_pos = 0;

// Put a character on screen
void putc(int c) {
    if (c == '\n') {
        // Move cursor to next line
        cursor_pos += 80 - (cursor_pos % 80);
    } else {
        vram[cursor_pos++] = (color << 8) | c;
    }

    // Simple scrolling when we reach bottom of screen (25 lines * 80 cols)
    if (cursor_pos >= 80 * 25) {
        // Scroll up by one line
        for (int i = 0; i < 80 * 24; i++) {
            vram[i] = vram[i + 80];
        }
        // Clear last line
        for (int i = 80 * 24; i < 80 * 25; i++) {
            vram[i] = (color << 8) | ' ';
        }
        cursor_pos -= 80;
    }
}

// Very simple print string function
void print(const char *str) {
    while (*str) {
        putc(*str++);
    }
}

// Very basic print decimal number (no negative numbers)
void print_dec(int num) {
    if (num == 0) {
        putc('0');
        return;
    }

    char buffer[10];
    int i = 0;

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    // Print digits in reverse order
    for (int j = i - 1; j >= 0; j--) {
        putc(buffer[j]);
    }
}

// A very simple printf-like function supporting %d and %s only
void simple_printf(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == 'd') {
                int val = __builtin_va_arg(args, int);
                print_dec(val);
            } else if (format[i] == 's') {
                char *val = __builtin_va_arg(args, char *);
                print(val);
            } else {
                putc('%');
                putc(format[i]);
            }
        } else {
            putc(format[i]);
        }
    }

    __builtin_va_end(args);
}

void main() {
    simple_printf("Neil OS Terminal Driver Test\n");
    simple_printf("Printing lines with scrolling...\n");

    for (int i = 0; i < 30; i++) {
        simple_printf("Line %d\n", i);
    }

    while (1) {
        uint8_t status = inb(0x64);

        if (status & 1) {
            uint8_t scancode = inb(0x60);
            // For now, we do nothing with scancode
        }
    }
}

