#include "keyboard.h"
#include "interrupt.h"
#include "terminal.h"
#include "rprintf.h"

// Keyboard scancode to ASCII mapping
unsigned char keyboard_map[128] = {
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
 '9', '0', '-', '=', '\b',     /* Backspace */
 '\t',                 /* Tab */
 'q', 'w', 'e', 'r',   /* 19 */
 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   0,                  /* 29   - Control */
 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
'\'', '`',   0,                /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
 'm', ',', '.', '/',   0,                              /* Right shift */
 '*',
   0,  /* Alt */
 ' ',  /* Space bar */
   0,  /* Caps lock */
   0,  /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,
   0,  /* < ... F10 */
   0,  /* 69 - Num lock*/
   0,  /* Scroll Lock */
   0,  /* Home key */
   0,  /* Up Arrow */
   0,  /* Page Up */
 '-',
   0,  /* Left Arrow */
   0,
   0,  /* Right Arrow */
 '+',
   0,  /* 79 - End key*/
   0,  /* Down Arrow */
   0,  /* Page Down */
   0,  /* Insert Key */
   0,  /* Delete Key */
   0,   0,   0,
   0,  /* F11 Key */
   0,  /* F12 Key */
   0,  /* All other keys are undefined */
};

// Read a byte from keyboard data port
uint8_t keyboard_read_data(void) {
    return inb(KEYBOARD_DATA_PORT);
}

// Read keyboard status register
uint8_t keyboard_read_status(void) {
    return inb(KEYBOARD_STATUS_PORT);
}

// Check if output buffer has data (LSB = 1)
int keyboard_has_data(void) {
    return (keyboard_read_status() & 0x01);
}

// Get scancode from keyboard (polling method)
uint8_t keyboard_get_scancode(void) {
    // Wait until data is available
    while (!keyboard_has_data()) {
        // Busy wait
    }
    return keyboard_read_data();
}

// Convert scancode to ASCII character
char scancode_to_ascii(uint8_t scancode) {
    if (scancode < 128) {
        return keyboard_map[scancode];
    }
    return 0;  // Key release or invalid scancode
}

// Keyboard interrupt handler
void keyboard_interrupt_handler(struct interrupt_frame* frame) {
    uint8_t scancode;
    char ascii;
    
    // Read the scancode from the keyboard
    scancode = keyboard_read_data();
    
    // Only process key press events (not key releases)
    if (scancode < 128) {
        ascii = scancode_to_ascii(scancode);
        
        if (ascii != 0) {
            // Echo the character to the terminal
            putc(ascii);
        }
        
        // Print scancode in hex for debugging
        esp_printf(putc, " [0x%x]", scancode);
    }
    
    // Send End of Interrupt signal to PIC
    PIC_sendEOI(1);  // IRQ 1 is keyboard
}

