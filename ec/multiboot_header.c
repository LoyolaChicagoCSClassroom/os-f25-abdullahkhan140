#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    0xe85250d6, 0, 24, 0x11ADAF12, 0, 0, 8
};

