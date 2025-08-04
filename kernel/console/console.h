#ifndef CONSOLE_CONSOLE_H
#define CONSOLE_CONSOLE_H

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
void move_cursor(unsigned short x, unsigned short y);
void set_color(Color fg, Color bg);
void clear();

}; // namespace console

#endif // CONSOLE_CONSOLE_H
