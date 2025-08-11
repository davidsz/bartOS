#ifndef SERIAL_SERIAL_H
#define SERIAL_SERIAL_H

// Communication on the serial port

namespace serial {

int init();
void write(const char *msg);

}; // namespace serial

#endif // SERIAL_SERIAL_H
