#ifndef OUTPUT_SERIAL_H
#define OUTPUT_SERIAL_H

// Communication on the serial port

namespace serial {

int init();
void write(const char *msg, ...);

}; // namespace serial

#endif // OUTPUT_SERIAL_H
