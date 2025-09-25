#include "rprintf.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>


static void putc(char c) {
    volatile char *video = (volatile char*)0xB8000; // VGA text buffer
    static size_t pos = 0;
    video[pos++] = c;
    video[pos++] = 0x07; // attribute
}

void rprintf(const char *str) {
    for (size_t i = 0; str[i]; i++) {
        putc(str[i]);
    }
}
