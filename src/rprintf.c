#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

static void putc(char c) {
    volatile char *video = (volatile char *)0xB8000;
    static size_t cursor = 0;
    video[cursor++] = c;
    video[cursor++] = 0x07; // white on black
}

static void puts(const char *str) {
    for (size_t i = 0; str[i]; i++) {
        putc(str[i]);
    }
}

static void print_num(int num, int base, int is_hex) {
    char buf[32];
    int i = 0;
    if (!num) buf[i++] = '0';
    while (num) {
        int rem = num % base;
        if (rem < 10) buf[i++] = '0' + rem;
        else buf[i++] = (is_hex ? 'A' : 'a') + rem - 10;
        num /= base;
    }
    for (int j = i-1; j >=0; j--) putc(buf[j]);
}

void rprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    for (size_t i = 0; fmt[i]; i++) {
        if (fmt[i] == '%') {
            i++;
            switch(fmt[i]) {
                case 'd': print_num(va_arg(args, int), 10, 0); break;
                case 'x': print_num(va_arg(args, unsigned int), 16, 1); break;
                case 'c': putc((char)va_arg(args, int)); break;
                case 's': puts(va_arg(args, const char*)); break;
                default: putc(fmt[i]); break;
            }
        } else {
            putc(fmt[i]);
        }
    }
    va_end(args);
}
