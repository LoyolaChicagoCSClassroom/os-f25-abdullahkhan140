#include "rprintf.h"
#include <stdint.h>

#define MULTIBOOT_HEADER_MAGIC    0x1BADB002
#define MULTIBOOT_HEADER_FLAGS    0x00000003
#define MULTIBOOT_CHECKSUM        -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

__attribute__((section(".multiboot"))) 
const uint32_t multiboot_header[] = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    MULTIBOOT_CHECKSUM
};

void kernel_main(void) {
    rprintf("Hello from kernel!\n");
    rprintf("Number: %d  Hex: %x  String: %s  Char: %c\n",
            1234, 0xBEEF, "test", 'A');
}
