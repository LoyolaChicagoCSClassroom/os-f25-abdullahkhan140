#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "interrupt.h"

// Keyboard I/O ports
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Function prototypes
uint8_t keyboard_read_data(void);
uint8_t keyboard_read_status(void);
int keyboard_has_data(void);
uint8_t keyboard_get_scancode(void);
char scancode_to_ascii(uint8_t scancode);
void keyboard_interrupt_handler(struct interrupt_frame* frame);

#endif

