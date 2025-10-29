#ifndef TERMINAL_H
#define TERMINAL_H

// Initialize the terminal driver
void terminal_init(void);

// Write a single character to the terminal
// This is the function you'll pass to esp_printf
int putc(int c);

// Clear the screen
void clear_screen(void);

#endif
