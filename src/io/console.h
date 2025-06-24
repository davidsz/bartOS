#ifndef IO_CONSOLE_H
#define IO_CONSOLE_H

// "Console" handles the framebuffer, a 80*25 sized memory-mapped output device

namespace io {

// Matches framebuffer colors
enum ConsoleColor : short int {
    Undefined = -1,
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

void console_print(const char *msg);
void move_cursor(unsigned short x, unsigned short y);
// void scroll_screen(unsigned int lines);
void set_console_color(ConsoleColor fg, ConsoleColor bg);
void console_clear();

}; // namespace io

#endif // IO_CONSOLE_H
