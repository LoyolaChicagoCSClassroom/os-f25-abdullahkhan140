#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
__attribute__((section(".multiboot"), used))
const unsigned int multiboot_header[] = {
    0xe85250d6,     
    0,              
    24,           
    0x11ADAF12,     
     0, 0, 8
};

#include <stdint.h>
#include "rprintf.h"
#include "interrupt.h"
#include "keyboard.h"

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_COLOR   0x07  // light gray on black
#define VGA_ADDR    0xB8000

// VGA text buffer (each cell = 2 bytes: [char, attribute])
static volatile uint16_t *const VGA_BUFFER = (uint16_t*)VGA_ADDR;

static int cursor_row = 0;
static int cursor_col = 0;

/* ------------------ VGA Text Output ------------------ */

static inline void vga_put_char_at(char ch, int row, int col) {
    VGA_BUFFER[row * VGA_WIDTH + col] = ((uint16_t)VGA_COLOR << 8) | (uint8_t)ch;
}

static void vga_scroll_if_needed(void) {
    if (cursor_row < VGA_HEIGHT)
        return;

    // Shift each line up
    for (int r = 1; r < VGA_HEIGHT; ++r) {
        for (int c = 0; c < VGA_WIDTH; ++c) {
            VGA_BUFFER[(r - 1) * VGA_WIDTH + c] = VGA_BUFFER[r * VGA_WIDTH + c];
        }
    }

    // Clear the last line
    for (int c = 0; c < VGA_WIDTH; ++c)
        vga_put_char_at(' ', VGA_HEIGHT - 1, c);

    cursor_row = VGA_HEIGHT - 1;
}

int putc(int ch) {
    char c = (char)ch;

    switch (c) {
        case '\n':
            cursor_row++;
            cursor_col = 0;
            break;
        case '\r':
            cursor_col = 0;
            break;
        case '\b':
            if (cursor_col > 0) {
                cursor_col--;
                vga_put_char_at(' ', cursor_row, cursor_col);
            } else if (cursor_row > 0) {
                cursor_row--;
                cursor_col = VGA_WIDTH - 1;
                vga_put_char_at(' ', cursor_row, cursor_col);
            }
            break;
        default:
            vga_put_char_at(c, cursor_row, cursor_col);
            if (++cursor_col >= VGA_WIDTH) {
                cursor_col = 0;
                cursor_row++;
            }
            break;
    }

    vga_scroll_if_needed();
    return 0;
}

/* ------------------ Utilities ------------------ */

static void delay_ms(int ms) {
    for (volatile int i = 0; i < ms * 100000; ++i);
}

/* ------------------ Kernel Entry ------------------ */

void kernel_main(void) {
    // Clear screen
    for (int r = 0; r < VGA_HEIGHT; ++r)
        for (int c = 0; c < VGA_WIDTH; ++c)
            vga_put_char_at(' ', r, c);

    esp_printf(putc, "===========================================\r\n");
    esp_printf(putc, "   Custom OS - COMP 310 Operating Systems  \r\n");
    esp_printf(putc, "===========================================\r\n\r\n");
    delay_ms(200);

    esp_printf(putc, "Initializing interrupt system...\r\n");
    remap_pic();
    load_gdt();
    init_idt();
    esp_printf(putc, "[OK] IDT & PIC ready\r\n");

    esp_printf(putc, "Enabling interrupts...\r\n");
    asm volatile("sti");
    esp_printf(putc, "[OK] IRQs enabled\r\n\r\n");

    esp_printf(putc, "Type 'help' for commands.\r\n\r\n");
    init_keyboard();   // keyboard driver should print the "$ " prompt

    // Idle loop; IRQ handlers take over
    while (1)
        asm volatile("hlt");
}
