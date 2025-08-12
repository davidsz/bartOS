#include "formatdevice.h"

namespace core {

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
                // TODO: Handle the unrecognized format
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

}; // namespace core
