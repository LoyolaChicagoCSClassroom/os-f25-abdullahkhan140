#include "rprintf.h"
#include <stdarg.h>

#define VGA_ADDRESS 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

static unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;
static int row = 0, col = 0;
static unsigned char color = 0x0F; // white on black

// --- minimal libc-like helpers ---
static int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

static char *itoa(int value, char *str, int base) {
    char *p = str;
    char *p1, *p2;
    unsigned int n = (value < 0 && base == 10) ? -value : (unsigned int)value;

    do {
        int rem = n % base;
        *p++ = (rem < 10) ? rem + '0' : rem - 10 + 'a';
        n /= base;
    } while (n);

    if (value < 0 && base == 10) {
        *p++ = '-';
    }

    *p = '\0';

    // reverse string
    for (p1 = str, p2 = p - 1; p1 < p2; p1++, p2--) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
    }
    return str;
}

// --- VGA output ---
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

    if (row >= VGA_ROWS) {
        row = 0; // simple wraparound
    }
}

static void puts(const char *s) {
    while (*s) {
        putc(*s++);
    }
}

// --- rprintf ---
void rprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                char *str = va_arg(args, char*);
                puts(str);
            } else if (*p == 'd') {
                int val = va_arg(args, int);
                char buf[32];
                itoa(val, buf, 10);
                puts(buf);
            } else if (*p == 'x') {
                int val = va_arg(args, int);
                char buf[32];
                itoa(val, buf, 16);
                puts(buf);
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                putc(c);
            } else {
                putc('%');
                putc(*p);
            }
        } else {
            putc(*p);
        }
    }

    va_end(args);
}
