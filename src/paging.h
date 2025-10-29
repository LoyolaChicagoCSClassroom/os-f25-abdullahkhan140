#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "page.h"

// Page Directory Entry structure
struct page_directory_entry {
   uint32_t present       : 1;   // Page present in memory
   uint32_t rw            : 1;   // Read-only if clear, R/W if set
   uint32_t user          : 1;   // Supervisor only if clear
   uint32_t writethru     : 1;   // Cache this directory as write-thru only
   uint32_t cachedisabled : 1;   // Disable cache on this page table?
   uint32_t accessed      : 1;   // Has the page been accessed since last refresh?
   uint32_t pagesize      : 1;   // 0 = 4KB pages, 1 = 4MB pages
   uint32_t ignored       : 2;   // Unused
   uint32_t os_specific   : 3;   // Available for OS use
   uint32_t frame         : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed));

// Page Table Entry structure
struct page {
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed));

// Function prototypes
void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd);
void init_paging(void);
void loadPageDirectory(struct page_directory_entry *pd);
void enable_paging(void);

// External symbols from linker
extern char _end_kernel;

#endif
