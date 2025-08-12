#include "console.h"
#include "core/io.h"
#include "formatdevice.h"
#include "vargs.h"
#include <stddef.h>
#include <stdint.h>

class FrameBuffer : public core::FormatDevice
{
public:
    // core::FormatDevice
    void PutChar(char c) override;
    void FormatProcessed() override;

    void WriteCell(size_t i, char c);
    void StartNewLine();
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

void FrameBuffer::PutChar(char c)
{
    if (c == '\n') {
        StartNewLine();
        return;
    }

    const size_t pos = m_y * m_width + m_x;
    WriteCell(pos, c);

    if (++m_x >= m_width)
        StartNewLine();
}

void FrameBuffer::FormatProcessed()
{
    // Move cursor to the end of the currently printed string
    MoveCursor(m_x, m_y);
}

void FrameBuffer::WriteCell(size_t pos, char c)
{
    // Format of framebuffer cells:
    // |__char__||_fg_||_bg_||__char__||_fg_||_bg_|...
    // | 1 byte ||  1 byte  || 1 byte ||  1 byte  |...

    pos *= 2;
    m_frameBufferPtr[pos] = c;
    m_frameBufferPtr[pos + 1] = ((m_fgColor & 0x0F) << 4) | (m_bgColor & 0x0F);
}

void FrameBuffer::StartNewLine()
{
    m_x = 0;
    m_y++;
    // Scroll if needed: move everything up by one line and create an empty last line
    if (m_y >= m_height) {
        const size_t line_size = m_width * 2;
        const size_t screen_size = m_length * 2;
        const size_t last_line = screen_size - line_size;
        for (size_t src = line_size; src < screen_size; src += 2) {
            size_t dest = src - line_size;
            m_frameBufferPtr[dest] = (uint16_t)m_frameBufferPtr[src];
            if (src >= last_line) {
                m_frameBufferPtr[src] = ' ';
                m_frameBufferPtr[src + 1] = ((m_fgColor & 0x0F) << 4) | (m_bgColor & 0x0F);
            }
        }
        m_y = m_height - 1;
    }
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

void print(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    print(msg, args);
    VA_END(args);
}

void print(const char *msg, VA_LIST args)
{
    s_frameBuffer.ProcessFormat(msg, args);
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
