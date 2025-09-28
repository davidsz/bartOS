#ifndef OUTPUT_CONSOLE_H
#define OUTPUT_CONSOLE_H

#include "vargs.h"
#include <stdint.h>

// "Console" handles the framebuffer, a 80*25 sized memory-mapped output device

namespace console {

// Matches framebuffer colors
enum Color : short int {
    Black = 0,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LightGrey,
    DarkGrey,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagenta,
    LightBrown,
    White
};

void print(const char *msg, ...);
void print(const char *msg, VA_LIST);
void backspace();
void move_cursor(unsigned short x, unsigned short y);
void set_color(Color fg, Color bg);
void clear();

}; // namespace console

#endif // OUTPUT_CONSOLE_H
