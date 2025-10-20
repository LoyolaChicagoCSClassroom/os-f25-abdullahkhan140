#include "page.h"

#define NUM_PAGES 128
#define PAGE_SIZE (2 * 1024 * 1024)  // 2 MB

struct ppage physical_page_array[NUM_PAGES];
struct ppage *free_physical_pages = NULL;

void init_pfa_list(void) {
    free_physical_pages = &physical_page_array[0];

    for (int i = 0; i < NUM_PAGES; i++) {
        physical_page_array[i].physical_addr = (void *)(i * PAGE_SIZE);
        physical_page_array[i].next = (i < NUM_PAGES - 1) ? &physical_page_array[i + 1] : NULL;
        physical_page_array[i].prev = (i > 0) ? &physical_page_array[i - 1] : NULL;
    }
}

struct ppage *allocate_physical_pages(unsigned int npages) {
    if (!free_physical_pages || npages == 0) return NULL;

    struct ppage *allocd_list = free_physical_pages;
    struct ppage *last = allocd_list;

    for (unsigned int i = 1; i < npages; i++) {
        if (!last->next) {
            return NULL; // Not enough free pages
        }
        last = last->next;
    }

    // Update free list
    free_physical_pages = last->next;
    if (free_physical_pages) {
        free_physical_pages->prev = NULL;
    }
    last->next = NULL;

    return allocd_list;
}

void free_physical_pages(struct ppage *ppage_list) {
    if (!ppage_list) return;

    // Find last page of the list
    struct ppage *last = ppage_list;
    while (last->next) {
        last = last->next;
    }

    // Insert at head of free list
    last->next = free_physical_pages;
    if (free_physical_pages) {
        free_physical_pages->prev = last;
    }
    free_physical_pages = ppage_list;
    ppage_list->prev = NULL;
}
