#include "rprintf.h"
#include <stdarg.h>
#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80

static uint16_t *vga_buffer = (uint16_t*)VGA_ADDRESS;
static size_t cursor_pos = 0;

static void putc(char c) {
    if (c == '\n') {
        cursor_pos += VGA_WIDTH - (cursor_pos % VGA_WIDTH);
        return;
    }
    vga_buffer[cursor_pos++] = (uint8_t)c | (0x0F << 8);
}

static void puts(const char *str) {
    for (size_t i = 0; str[i]; i++) {
        putc(str[i]);
    }
}

static void print_num(unsigned int num, int base, int is_hex) {
    char buf[32];
    int i = 0;
    if (num == 0) {
        buf[i++] = '0';
    } else {
        while (num) {
            int rem = num % base;
            if (rem < 10) buf[i++] = '0' + rem;
            else buf[i++] = (is_hex ? 'A' : 'a') + rem - 10;
            num /= base;
        }
    }
    while (--i >= 0) putc(buf[i]);
}

void rprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    for (size_t i = 0; fmt[i]; i++) {
        if (fmt[i] != '%') {
            putc(fmt[i]);
            continue;
        }
        i++;
        switch (fmt[i]) {
            case 'd': print_num(va_arg(args, int), 10, 0); break;
            case 'x': print_num(va_arg(args, unsigned int), 16, 1); break;
            case 'c': putc((char)va_arg(args, int)); break;
            case 's': puts(va_arg(args, const char*)); break;
            default: putc(fmt[i]); break;
        }
    }
    va_end(args);
}
