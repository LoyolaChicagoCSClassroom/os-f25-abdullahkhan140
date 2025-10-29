#include "page.h"

#define PAGE_SIZE (2 * 1024 * 1024)  // 2 MB pages
#define NUM_PAGES 128                 // 128 pages = 256 MB total

// Statically allocate array of physical page structures
struct ppage physical_page_array[NUM_PAGES];

// Global pointer to the head of the free pages list
struct ppage *free_page_list = 0;

/*
 * init_pfa_list
 *
 * Initialize the page frame allocator by linking all pages into the free list.
 * Each page represents a 2MB block of physical memory.
 */
void init_pfa_list(void) {
    int i;
    
    // Start physical memory at 2MB (skip first 2MB for kernel code/data)
    unsigned long physical_address = 0x200000;  // Start at 2MB
    
    // Initialize the first page
    physical_page_array[0].next = (NUM_PAGES > 1) ? &physical_page_array[1] : 0;
    physical_page_array[0].prev = 0;
    physical_page_array[0].physical_addr = (void *)physical_address;
    
    // Link all pages together in a doubly-linked list
    for (i = 1; i < NUM_PAGES - 1; i++) {
        physical_page_array[i].next = &physical_page_array[i + 1];
        physical_page_array[i].prev = &physical_page_array[i - 1];
        physical_page_array[i].physical_addr = (void *)(physical_address + (i * PAGE_SIZE));
    }
    
    // Initialize the last page
    if (NUM_PAGES > 1) {
        physical_page_array[NUM_PAGES - 1].next = 0;
        physical_page_array[NUM_PAGES - 1].prev = &physical_page_array[NUM_PAGES - 2];
        physical_page_array[NUM_PAGES - 1].physical_addr = 
            (void *)(physical_address + ((NUM_PAGES - 1) * PAGE_SIZE));
    }
    
    // Set the global free list pointer to the head
    free_page_list = &physical_page_array[0];
}

/*
 * allocate_physical_pages
 *
 * Allocates npages physical pages from the free list.
 * Returns a pointer to a new list containing the allocated pages.
 * Returns 0 if not enough pages are available.
 */
struct ppage *allocate_physical_pages(unsigned int npages) {
    struct ppage *allocated_list = 0;
    struct ppage *current;
    unsigned int i;
    
    // Check if we have enough free pages
    if (free_page_list == 0 || npages == 0) {
        return 0;
    }
    
    // Count available pages
    unsigned int available = 0;
    current = free_page_list;
    while (current != 0) {
        available++;
        current = current->next;
    }
    
    if (available < npages) {
        return 0;  // Not enough pages available
    }
    
    // Unlink npages from the free list
    allocated_list = free_page_list;
    current = free_page_list;
    
    // Move npages-1 steps forward to find the last page to allocate
    for (i = 0; i < npages - 1; i++) {
        current = current->next;
    }
    
    // Update the free list to point to the next page after our allocation
    free_page_list = current->next;
    
    if (free_page_list != 0) {
        free_page_list->prev = 0;  // New head has no previous
    }
    
    // Terminate the allocated list
    current->next = 0;
    
    return allocated_list;
}

/*
 * free_physical_pages
 *
 * Returns a list of physical pages back to the free list.
 * The pages are prepended to the beginning of the free list.
 */
void free_physical_pages(struct ppage *ppage_list) {
    struct ppage *current;
    
    if (ppage_list == 0) {
        return;  // Nothing to free
    }
    
    // Find the end of the list we're freeing
    current = ppage_list;
    while (current->next != 0) {
        current = current->next;
    }
    
    // Link the end of ppage_list to the current free list
    current->next = free_page_list;
    
    if (free_page_list != 0) {
        free_page_list->prev = current;
    }
    
    // Update the free list head
    ppage_list->prev = 0;
    free_page_list = ppage_list;
}
