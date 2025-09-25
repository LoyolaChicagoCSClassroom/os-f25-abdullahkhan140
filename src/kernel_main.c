#include "rprintf.h"

void kernel_main(void) {
    rprintf("line 1\n");
    rprintf("line 2\n");
    rprintf("line 3\n");
    rprintf("line 4\n");
    rprintf("line 5\n");
    while(1) {} // keep kernel running
}
