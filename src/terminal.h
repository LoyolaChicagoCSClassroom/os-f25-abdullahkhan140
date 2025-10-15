#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

// Initialize terminal
void terminal_initialize();

// Write a single character to the screen
void terminal_write_char(char c);

// Write a null-terminated string to the screen
void terminal_write_string(const char* str);

#endif
