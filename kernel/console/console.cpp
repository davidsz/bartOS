#include "console.h"
#include "core/io.h"
#include "vargs.h"
#include <stddef.h>
#include <stdint.h>

class FrameBuffer
{
public:
    void Print(const char *msg, VA_LIST args);
    void WriteCell(size_t i, char c);
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

    // Write the given type to the console, then return the number
    // of the characters written
    int WriteDigits(size_t start, int d);
    int WriteHex32(size_t start, uint32_t h);
};

static FrameBuffer s_frameBuffer;

void FrameBuffer::Print(const char *msg, VA_LIST args)
{
    int x = m_x;
    int y = m_y;
    for (int i = 0; msg[i] != '\0'; i++) {
        if (msg[i] == '\n') {
            x = 0;
            y++;
            continue;
        }
        unsigned int pos = y * m_width + x;
        if (msg[i] == '%' && msg[i + 1] != '\0') {
            switch(msg[i + 1]) {
            case 'd': {
                int d = VA_ARG(args, int);
                x += WriteDigits(pos, d);
                i++;
                continue;
            }
            case 'p': {
                uint32_t h = VA_ARG(args, uint32_t);
                x += WriteHex32(pos, h);
                i++;
                continue;
            }
            default: {}
            }
        }
        s_frameBuffer.WriteCell(pos, msg[i]);
        x++;
    }
    s_frameBuffer.MoveCursor(x, y);
}

void FrameBuffer::WriteCell(size_t i, char c)
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

int FrameBuffer::WriteDigits(size_t pos, int d)
{
    if (d == 0) {
        WriteCell(pos, '0');
        return 1;
    }

    int count = 0;
    if (d < 0) {
        WriteCell(pos, '-');
        pos++;
        count++;
        d = -d;
    }

    // 32 bit int is 11 digits long maximum
    const int bufsize = 11;
    char buf[bufsize];
    int i = bufsize;
    while (d > 0 && i > 0) {
        buf[--i] = (d % 10) + '0';
        d = d / 10;
    }
    while(i < bufsize) {
        WriteCell(pos, buf[i++]);
        pos++;
        count++;
    }

    return count;
}

int FrameBuffer::WriteHex32(size_t start, uint32_t h)
{
    char res[] = "0x00000000";
    const char *charset = "0123456789ABCDEF";
    for (int i = 2, shift = 28; i < 10; i++, shift -= 4)
        res[i] = charset[(h >> shift) & 0xF];
    for (int i = 0; i < 10; i++)
        WriteCell(start + i, res[i]);
    return 10;
}

namespace console {

void print(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);

    s_frameBuffer.Print(msg, args);

    VA_END(args);
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

void printfHex(uint8_t key) {
    char res[] = "00";
    const char *charset = "0123456789ABCDEF";
    res[0] = charset[(key >> 4) & 0xF];
    res[1] = charset[key & 0xF];
    print(res);
}

void print_hex32(uint32_t key)
{
    print("0x");
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex(key & 0xFF);
    print("\n");
}

}; // namespace console
