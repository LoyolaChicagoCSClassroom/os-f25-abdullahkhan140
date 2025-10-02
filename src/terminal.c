// terminal.c
#include "terminal.h"
void putc(char c) {
    volatile char *video = (volatile char *)0xB8000;
    static int pos = 0;

    video[pos++] = c;
    video[pos++] = 0x07;
}
