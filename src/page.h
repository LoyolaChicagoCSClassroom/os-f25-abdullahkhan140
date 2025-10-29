#ifndef PAGE_H
#define PAGE_H

// Structure to track each physical page
struct ppage {
    struct ppage *next;
    struct ppage *prev;
    void *physical_addr;
};

// Initialize the page frame allocator
void init_pfa_list(void);

// Allocate npages physical pages from the free list
struct ppage *allocate_physical_pages(unsigned int npages);

// Free a list of physical pages back to the free list
void free_physical_pages(struct ppage *ppage_list);

// Global pointer to the free pages list
extern struct ppage *free_page_list;

#endif
