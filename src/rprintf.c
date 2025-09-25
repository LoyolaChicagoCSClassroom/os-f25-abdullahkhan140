#include "rprintf.h"
#include <stdarg.h>
#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

static unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;
static int row = 0, col = 0;
static unsigned char color = 0x0F; // white on black

// Multiboot header
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    MULTIBOOT_CHECKSUM
};

// Helper: strlen
static int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

// Helper: integer to string
static char *itoa(int value, char *str, int base) {
    char *p = str, *p1, *p2;
    unsigned int n = (value < 0 && base == 10) ? -value : (unsigned int)value;

    do {
        int rem = n % base;
        *p++ = (rem < 10) ? rem + '0' : rem - 10 + 'a';
        n /= base;
    } while (n);

    if (value < 0 && base == 10) *p++ = '-';
    *p = '\0';

    for (p1 = str, p2 = p - 1; p1 < p2; p1++, p2--) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
    }
    return str;
}

// Output a single character
static void putc(char c) {
    if (c == '\n') {
        row++;
        col = 0;
    } else {
        int index = row * VGA_COLS + col;
        vga_buffer[index] = (unsigned short)c | (unsigned short)(color << 8);
        col++;
        if (col >= VGA_COLS) {
            col = 0;
            row++;
        }
    }
    if (row >= VGA_ROWS) row = 0;
}

// Output a string
static void puts(const char *s) {
    while (*s) putc(*s++);
}

// rprintf implementation (supports %d, %x, %c, %s)
void rprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[32];

    for (const char *p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': itoa(va_arg(args, int), buf, 10); puts(buf); break;
                case 'x': itoa(va_arg(args, int), buf, 16); puts(buf); break;
                case 'c': putc((char)va_arg(args, int)); break;
                case 's': puts(va_arg(args, char*)); break;
                default: putc(*p); break;
            }
        } else {
            putc(*p);
        }
    }
    va_end(args);
}
