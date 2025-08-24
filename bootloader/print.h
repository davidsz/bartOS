#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdint.h>

typedef char *VA_LIST;

#define VA_START(p, arg) (p = (VA_LIST)&arg + sizeof(arg))

#define VA_ARG(p, type) (*(type *)((p += sizeof(type)) - sizeof(type)))

#define VA_END(p) (p = 0)

// Base class for printf-like functionalities
class FormatDevice {
public:
    // Handle a single character
    virtual void PutChar(char c) = 0;
    // Notifies about the end of the format processing
    virtual void FormatProcessed() {}

    // The core algorithm for processing the format string
    void ProcessFormat(const char *format, VA_LIST args);
private:
    void ProcessDigits(int d);
    void ProcessHex32(uint32_t h);
};

void FormatDevice::ProcessFormat(const char *format, VA_LIST args)
{
    for (int i = 0; format[i] != '\0'; i++) {
        // Handle format placeholders
        if (format[i] == '%' && format[i + 1] != '\0') {
            switch(format[i + 1]) {
            // A single char
            case 'c': {
                PutChar(VA_ARG(args, char));
                i++;
                continue;
            }
            // Decimal digits
            case 'i':
            case 'd': {
                ProcessDigits(VA_ARG(args, int));
                i++;
                continue;
            }
            // Pointer
            case 'p': {
                ProcessHex32(VA_ARG(args, uint32_t));
                i++;
                continue;
            }
            // String
            case 's': {
                char *s = VA_ARG(args, char *);
                while (*s != '\0')
                    PutChar(*s++);
                i++;
                continue;
            }
            default: {
                FormatProcessed();
                return;
            }
            }
        }
        PutChar(format[i]);
    }
    FormatProcessed();
}

void FormatDevice::ProcessDigits(int d)
{
    if (d == 0) {
        PutChar('0');
        return;
    } else if (d < 0) {
        PutChar('-');
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
    while(i < bufsize)
        PutChar(buf[i++]);
}

void FormatDevice::ProcessHex32(uint32_t h)
{
    char res[] = "0x00000000";
    const char *charset = "0123456789ABCDEF";
    for (int i = 2, shift = 28; i < 10; i++, shift -= 4)
        res[i] = charset[(h >> shift) & 0xF];
    for (int i = 0; i < 10; i++)
        PutChar(res[i]);
}

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
void move_cursor(unsigned short x, unsigned short y);
void set_color(Color fg, Color bg);
void clear();

class FrameBuffer : public FormatDevice
{
public:
    // core::FormatDevice
    void PutChar(char c) override;
    void FormatProcessed() override;

    void WriteCell(size_t i, char c);
    void StartNewLine();
    void MoveCursor(unsigned short x, unsigned short y);
    void SetConsoleColor(Color fg, Color bg);
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
    Color m_fgColor = Color::White;
    Color m_bgColor = Color::Black;
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

void FrameBuffer::MoveCursor(unsigned short, unsigned short)
{
}

void FrameBuffer::SetConsoleColor(Color fg, Color bg)
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

#endif
