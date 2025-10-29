#include "paging.h"
#include "page.h"
#include "rprintf.h"
#include "terminal.h"

// Page directory and page table - must be 4KB aligned and global
struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));

#define PAGE_SIZE 4096
#define ENTRIES_PER_TABLE 1024

/*
 * map_pages
 *
 * Maps a list of physical pages to a specified virtual address.
 *
 * Parameters:
 *   vaddr - Virtual address to map to
 *   pglist - Linked list of physical pages from page frame allocator
 *   pd - Page directory (root of page table)
 *
 * Returns: The virtual address that was mapped
 */
void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd) {
    uint32_t virtual_addr = (uint32_t)vaddr;
    struct ppage *current_page = pglist;
    
    while (current_page != 0) {
        // Calculate page directory index (top 10 bits of virtual address)
        uint32_t pd_index = virtual_addr >> 22;
        
        // Calculate page table index (middle 10 bits of virtual address)
        uint32_t pt_index = (virtual_addr >> 12) & 0x3FF;
        
        // Check if page directory entry is present
        if (!pd[pd_index].present) {
            // Initialize the page directory entry to point to our page table
            pd[pd_index].present = 1;
            pd[pd_index].rw = 1;           // Read/Write
            pd[pd_index].user = 0;         // Supervisor mode
            pd[pd_index].writethru = 0;
            pd[pd_index].cachedisabled = 0;
            pd[pd_index].accessed = 0;
            pd[pd_index].pagesize = 0;     // 4KB pages
            pd[pd_index].ignored = 0;
            pd[pd_index].os_specific = 0;
            // Point to our page table (physical address)
            pd[pd_index].frame = ((uint32_t)pt) >> 12;
        }
        
        // Now set up the page table entry
        pt[pt_index].present = 1;
        pt[pt_index].rw = 1;              // Read/Write
        pt[pt_index].user = 0;            // Supervisor mode
        pt[pt_index].accessed = 0;
        pt[pt_index].dirty = 0;
        pt[pt_index].unused = 0;
        // Set the physical frame address
        pt[pt_index].frame = ((uint32_t)current_page->physical_addr) >> 12;
        
        // Move to next page in the list
        current_page = current_page->next;
        
        // Move to next virtual page (4KB forward)
        virtual_addr += PAGE_SIZE;
    }
    
    return vaddr;
}

/*
 * loadPageDirectory
 *
 * Load the page directory address into CR3 register
 */
void loadPageDirectory(struct page_directory_entry *pd) {
    asm volatile("mov %0, %%cr3"
        :
        : "r"(pd)
        : "memory");
}

/*
 * enable_paging
 *
 * Enable paging by setting bits 0 and 31 of CR0
 */
void enable_paging(void) {
    asm volatile(
        "mov %%cr0, %%eax\n"
        "or $0x80000001, %%eax\n"
        "mov %%eax, %%cr0"
        :
        :
        : "eax", "memory");
}

/*
 * init_paging
 *
 * Initialize paging by identity mapping the kernel, stack, and video memory
 */
void init_paging(void) {
    uint32_t kernel_start = 0x100000;  // 1MB - where kernel starts
    uint32_t kernel_end = 0x400000;
    uint32_t current_addr;
    struct ppage tmp;
    uint32_t esp;
    int i;
    
    esp_printf(putc, "[HW4] Initializing paging...\r\n");
    
    // Initialize page directory to all zeros
    for (i = 0; i < 1024; i++) {
        pd[i].present = 0;
        pd[i].rw = 0;
        pd[i].user = 0;
        pd[i].writethru = 0;
        pd[i].cachedisabled = 0;
        pd[i].accessed = 0;
        pd[i].pagesize = 0;
        pd[i].ignored = 0;
        pd[i].os_specific = 0;
        pd[i].frame = 0;
    }
    
    // Initialize page table to all zeros
    for (i = 0; i < 1024; i++) {
        pt[i].present = 0;
        pt[i].rw = 0;
        pt[i].user = 0;
        pt[i].accessed = 0;
        pt[i].dirty = 0;
        pt[i].unused = 0;
        pt[i].frame = 0;
    }
    
    esp_printf(putc, "[HW4] Identity mapping kernel from 0x%x to 0x%x...\r\n", 
               kernel_start, kernel_end);
    
    // Identity map the kernel (from 0x100000 to &_end_kernel)
    for (current_addr = kernel_start; current_addr < kernel_end; current_addr += PAGE_SIZE) {
        tmp.next = 0;
        tmp.physical_addr = (void *)current_addr;
        map_pages((void *)current_addr, &tmp, pd);
    }
    
    // Get current stack pointer
    asm("mov %%esp, %0" : "=r" (esp));
    
    esp_printf(putc, "[HW4] Identity mapping stack at ESP=0x%x...\r\n", esp);
    
    // Identity map the stack (round down to page boundary)
    uint32_t stack_page = esp & 0xFFFFF000;  // Round down to 4KB boundary
    // Map a few pages for the stack (stack grows down, so map below ESP too)
    for (i = 0; i < 4; i++) {  // Map 4 pages (16KB) for stack
        tmp.next = 0;
        tmp.physical_addr = (void *)(stack_page - (i * PAGE_SIZE));
        map_pages((void *)(stack_page - (i * PAGE_SIZE)), &tmp, pd);
    }
    
    esp_printf(putc, "[HW4] Identity mapping video memory at 0xB8000...\r\n");
    
    // Identity map video memory (0xB8000)
    tmp.next = 0;
    tmp.physical_addr = (void *)0xB8000;
    map_pages((void *)0xB8000, &tmp, pd);
    
    esp_printf(putc, "[HW4] Loading page directory into CR3...\r\n");
    
    // Load page directory
    loadPageDirectory(pd);
    
    esp_printf(putc, "[HW4] Enabling paging...\r\n");
    
    // Enable paging
    enable_paging();
    
    esp_printf(putc, "[HW4] Paging enabled successfully!\r\n\r\n");
}
