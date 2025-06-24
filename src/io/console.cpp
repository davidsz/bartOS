#include "console.h"
#include "io.h"


class FrameBuffer
{
public:
    void WriteCell(unsigned int i, char c,
                   io::ConsoleColor fg = io::ConsoleColor::Undefined,
                   io::ConsoleColor bg = io::ConsoleColor::Undefined);
    void MoveCursor(unsigned short x, unsigned short y);
    void SetConsoleColor(io::ConsoleColor fg, io::ConsoleColor bg);
    void Clear();

private:
    enum Port {
        Command = 0x3D4,
        Data = 0x3D5
    };

    enum PortCommand {
        HighByte = 14,
        LowByte = 15
    };

    // Start of framebuffer
    char *m_frameBufferPtr = (char *)0x000B8000;

    // Sizes of the framebuffer as console
    const unsigned int m_width = 80;
    const unsigned int m_height = 25;
    const unsigned int m_length = m_width * m_height;

    // Current cursor position
    unsigned short m_x = 0;
    unsigned short m_y = 0;

    // Current console colors
    io::ConsoleColor m_fgColor = io::White;
    io::ConsoleColor m_bgColor = io::Black;
};

static FrameBuffer s_frameBuffer;

/*
 *  @param i   Index in the framebuffer array
 *  @param c   The character
 *  @param fg  Foreground color
 *  @param bg  Background color
 */
void FrameBuffer::WriteCell(unsigned int i, char c, io::ConsoleColor fg, io::ConsoleColor bg)
{
    // Format of framebuffer cells:
    // |__char__||_fg_||_bg_||__char__||_fg_||_bg_|...
    // | 1 byte ||  1 byte  || 1 byte ||  1 byte  |...

    fg = fg == io::ConsoleColor::Undefined ? m_fgColor : fg;
    bg = bg == io::ConsoleColor::Undefined ? m_bgColor : bg;

    i *= 2;
    m_frameBufferPtr[i] = c;
    m_frameBufferPtr[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void FrameBuffer::MoveCursor(unsigned short x, unsigned short y)
{
    if (x >= m_width || y >= m_height)
        return;

    m_x = x;
    m_y = y;

    int pos = y * m_width + x;
    io::outb(Port::Command, PortCommand::HighByte);
    io::outb(Port::Data,    ((pos >> 8) & 0x00FF));
    io::outb(Port::Command, PortCommand::LowByte);
    io::outb(Port::Data,    pos & 0x00FF);
}

void FrameBuffer::SetConsoleColor(io::ConsoleColor fg, io::ConsoleColor bg)
{
    m_fgColor = fg;
    m_bgColor = bg;
}

void FrameBuffer::Clear()
{
    for (unsigned int i = 0; i < m_length; i++)
        WriteCell(i, ' ');
    MoveCursor(0, 0);
}

namespace io {

void console_print(const char *msg)
{
    int i = 0;
    for (; msg[i] != '\0'; i++)
        s_frameBuffer.WriteCell(i, msg[i]);

    s_frameBuffer.MoveCursor(i, 0);
}

void move_cursor(unsigned short x, unsigned short y)
{
    s_frameBuffer.MoveCursor(x, y);
}

void set_console_color(ConsoleColor fg, ConsoleColor bg)
{
    s_frameBuffer.SetConsoleColor(fg, bg);
}

void console_clear()
{
    s_frameBuffer.Clear();
}

}; // namespace io
