#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    0xe85250d6,     
    0,              
    24,           
    0x11ADAF12,     
     0, 0, 8
};

#include "page.h"
#include <stdio.h>

int main(void) {
    printf("Initializing Page Frame Allocator...\n");
    init_pfa_list();

    printf("Allocating 5 pages...\n");
    struct ppage *alloc = allocate_physical_pages(5);

    printf("Allocated pages:\n");
    struct ppage *cur = alloc;
    while (cur) {
        printf("Page at physical address: %p\n", cur->physical_addr);
        cur = cur->next;
    }

    printf("Freeing allocated pages...\n");
    free_physical_pages(alloc);

    // Check free list
    printf("Free list after freeing pages:\n");
    cur = free_physical_pages;
    int count = 0;
    while (cur) {
        count++;
        cur = cur->next;
    }
    printf("Total free pages: %d\n", count);

    return 0;
}
