#include "rprintf.h"

void kernel_main(void) {
    rprintf("Hello, bare-metal world!\n");
    rprintf("Decimal: %d\n", 1234);
    rprintf("Hex: %x\n", 0xABCD);
    rprintf("String: %s\n", "sample");
    rprintf("Char: %c\n", 'X');
}
