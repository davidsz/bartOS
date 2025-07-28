#include "console.h"
#include "core/io.h"


class FrameBuffer
{
public:
    void Print(const char *msg);
    void WriteCell(unsigned int i, char c);
    void MoveCursor(unsigned short x, unsigned short y);
    void SetConsoleColor(console::Color fg, console::Color bg);
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
    console::Color m_fgColor = console::Color::White;
    console::Color m_bgColor = console::Color::Black;
};

static FrameBuffer s_frameBuffer;

void FrameBuffer::Print(const char *msg)
{
    int x = m_x;
    int y = m_y;
    for (int i = 0; msg[i] != '\0'; i++) {
        if (msg[i] == '\n') {
            x = 0;
            y++;
            continue;
        }
        s_frameBuffer.WriteCell(y * m_width + x, msg[i]);
        x++;
    }
    s_frameBuffer.MoveCursor(x, y);
}

void FrameBuffer::WriteCell(unsigned int i, char c)
{
    // Format of framebuffer cells:
    // |__char__||_fg_||_bg_||__char__||_fg_||_bg_|...
    // | 1 byte ||  1 byte  || 1 byte ||  1 byte  |...

    i *= 2;
    m_frameBufferPtr[i] = c;
    m_frameBufferPtr[i + 1] = ((m_fgColor & 0x0F) << 4) | (m_bgColor & 0x0F);
}

void FrameBuffer::MoveCursor(unsigned short x, unsigned short y)
{
    if (x >= m_width || y >= m_height)
        return;

    m_x = x;
    m_y = y;

    int pos = y * m_width + x;
    core::outb(Port::Command, PortCommand::HighByte);
    core::outb(Port::Data,    ((pos >> 8) & 0x00FF));
    core::outb(Port::Command, PortCommand::LowByte);
    core::outb(Port::Data,    pos & 0x00FF);
}

void FrameBuffer::SetConsoleColor(console::Color fg, console::Color bg)
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

namespace console {

void print(const char *msg)
{
    s_frameBuffer.Print(msg);
}

void move_cursor(unsigned short x, unsigned short y)
{
    s_frameBuffer.MoveCursor(x, y);
}

void set_color(Color fg, Color bg)
{
    s_frameBuffer.SetConsoleColor(fg, bg);
}

void clear()
{
    s_frameBuffer.Clear();
}

}; // namespace console
