#ifndef OUTPUT_FORMATDEVICE_H
#define OUTPUT_FORMATDEVICE_H

#include "vargs.h"
#include <stdint.h>

namespace core {

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

}; // namespace core

#endif // OUTPUT_FORMATDEVICE_H
