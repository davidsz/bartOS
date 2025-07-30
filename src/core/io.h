#ifndef CORE_IO_H
#define CORE_IO_H

#include <stdint.h>

namespace core {

// Reads one or two bytes from the given I/O port. (defined in io.s)
extern "C" uint8_t inb(uint16_t port);
extern "C" uint16_t inw(uint16_t port);

// Sends the given data to the given I/O port. (defined in io.s)
extern "C" void outb(uint16_t port, uint8_t data);
extern "C" void outw(uint16_t port, uint16_t data);

};

#endif // CORE_IO_H
