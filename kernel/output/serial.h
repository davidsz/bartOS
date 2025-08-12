#ifndef OUTPUT_SERIAL_H
#define OUTPUT_SERIAL_H

#include "vargs.h"

// Communication on the serial port

namespace serial {

int init();
void write(const char *msg, ...);
void write(const char *msg, VA_LIST);

}; // namespace serial

#endif // OUTPUT_SERIAL_H
