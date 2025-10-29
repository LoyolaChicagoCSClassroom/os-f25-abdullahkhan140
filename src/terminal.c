#include "terminal.h"
#include "rprintf.h"

// Video memory starts at 0xB8000
#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDEO_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2)

// Color: light grey on black
#define DEFAULT_COLOR 0x07

// Current cursor position
static unsigned int cursor_offset = 0;

// Helper function to get pointer to video memory
static inline unsigned short* get_video_memory(void) {
    return (unsigned short*)VIDEO_MEMORY;
}

// Scroll the screen up by one line
static void scroll_screen(void) {
    unsigned short* video = get_video_memory();
    int i;
    
    // Copy each line to the previous line (line 1 -> line 0, etc.)
    for (i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
        video[i] = video[i + SCREEN_WIDTH];
    }
    
    // Clear the last line
    for (i = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
        video[i] = (DEFAULT_COLOR << 8) | ' ';
    }
    
    // Move cursor to beginning of last line
    cursor_offset = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH;
}

// Write a single character to the terminal
int putc(int c) {
    unsigned short* video = get_video_memory();
    unsigned short attribute = DEFAULT_COLOR << 8;
    
    // Handle special characters
    if (c == '\n' || c == 0x0A) {
        // Newline - move to beginning of next line
        cursor_offset = ((cursor_offset / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;
    } else if (c == '\r' || c == 0x0D) {
        // Carriage return - move to beginning of current line
        cursor_offset = (cursor_offset / SCREEN_WIDTH) * SCREEN_WIDTH;
    } else if (c == '\b' || c == 0x08) {
        // Backspace - move back one position
        if (cursor_offset > 0) {
            cursor_offset--;
            video[cursor_offset] = attribute | ' ';
        }
    } else if (c == '\t') {
        // Tab - move to next multiple of 8
        cursor_offset = (cursor_offset + 8) & ~7;
    } else {
        // Regular character - write it to video memory
        video[cursor_offset] = attribute | (unsigned char)c;
        cursor_offset++;
    }
    
    // Check if we need to scroll
    if (cursor_offset >= SCREEN_WIDTH * SCREEN_HEIGHT) {
        scroll_screen();
    }
    
    return c;
}

// Clear the screen
void clear_screen(void) {
    unsigned short* video = get_video_memory();
    unsigned short blank = (DEFAULT_COLOR << 8) | ' ';
    int i;
    
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video[i] = blank;
    }
    
    cursor_offset = 0;
}

// Initialize the terminal
void terminal_init(void) {
    clear_screen();
}
