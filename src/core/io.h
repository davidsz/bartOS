#ifndef CORE_IO_H
#define CORE_IO_H

namespace core {

/*
*  Sends the given data to the given I/O port. Defined in io.s
*
*  @param port The I/O port to send the data to
*  @param data The data to send to the I/O port
*/
extern "C" void outb(unsigned short port, unsigned char data);

};

#endif // CORE_IO_H
