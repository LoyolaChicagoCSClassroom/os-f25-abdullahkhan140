#include "rprintf.h"

void kernel_main(void) {
    rprintf("Hello from kernel!\n");
    rprintf("Number: %d  Hex: %x  String: %s  Char: %c\n", 1234, 0xBEEF, "test", 'A');
}
