#include "rprintf.h"
#include "terminal.h"
#include "interrupt.h"
#include "keyboard.h"
#include "page.h"
#include "paging.h"

// Get current execution level (privilege level)
static inline unsigned int get_execution_level(void) {
    unsigned int cs;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    return cs & 0x3;  // Bottom 2 bits contain the privilege level
}

void main(void) {
    unsigned int exec_level;
    struct ppage *allocated_pages;
    struct ppage *current;
    int count;
    
    // Initialize the terminal (HW1)
    terminal_init();
    
    esp_printf(putc, "=================================\r\n");
    esp_printf(putc, "CS 310 Operating System\r\n");
    esp_printf(putc, "=================================\r\n\r\n");
    
    // HW1: Print execution level
    exec_level = get_execution_level();
    esp_printf(putc, "[HW1] Current Execution Level (CPL): %d\r\n\r\n", exec_level);
    
    // HW2: Initialize interrupts
    esp_printf(putc, "[HW2] Initializing interrupts...\r\n");
    remap_pic();   // Remap the Programmable Interrupt Controller
    load_gdt();    // Load the Global Descriptor Table
    init_idt();    // Initialize the Interrupt Descriptor Table
    
    esp_printf(putc, "[HW2] Interrupts initialized\r\n");
    esp_printf(putc, "[HW2] Enabling keyboard interrupts...\r\n\r\n");
    
    // Enable interrupts
    __asm__ volatile("sti");
    
    // HW3: Initialize and test page frame allocator
    esp_printf(putc, "[HW3] Initializing Page Frame Allocator...\r\n");
    init_pfa_list();
    esp_printf(putc, "[HW3] Page Frame Allocator initialized\r\n");
    esp_printf(putc, "[HW3] Total pages: 128 (256 MB total memory)\r\n\r\n");
    
    // Test: Allocate 5 pages
    esp_printf(putc, "[HW3] Test: Allocating 5 physical pages...\r\n");
    allocated_pages = allocate_physical_pages(5);
    
    if (allocated_pages != 0) {
        esp_printf(putc, "[HW3] Successfully allocated 5 pages:\r\n");
        current = allocated_pages;
        count = 0;
        while (current != 0 && count < 5) {
            esp_printf(putc, "  Page %d: Physical Address = 0x%x\r\n", 
                      count, (unsigned int)current->physical_addr);
            current = current->next;
            count++;
        }
        esp_printf(putc, "\r\n");
        
        // Test: Free the pages back
        esp_printf(putc, "[HW3] Test: Freeing 5 pages back to free list...\r\n");
        free_physical_pages(allocated_pages);
        esp_printf(putc, "[HW3] Pages successfully freed\r\n\r\n");
    } else {
        esp_printf(putc, "[HW3] ERROR: Failed to allocate pages!\r\n\r\n");
    }
    
    // HW4: Initialize paging (virtual memory)
    init_paging();
    
    // Test that paging works - if we get here, paging is working!
    esp_printf(putc, "[HW4] Paging test: Writing to screen after paging enabled\r\n");
    esp_printf(putc, "[HW4] If you can read this, paging is working!\r\n\r\n");
    
    esp_printf(putc, "=================================\r\n");
    esp_printf(putc, "System Ready - Type on keyboard!\r\n");
    esp_printf(putc, "=================================\r\n\r\n");
    
    // Infinite loop - kernel doesn't exit
    // Keyboard interrupts will be handled automatically
    while(1) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}
