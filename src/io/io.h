#ifndef IO_IO_H
#define IO_IO_H

namespace io {

/*
*  Sends the given data to the given I/O port. Defined in io.s
*
*  @param port The I/O port to send the data to
*  @param data The data to send to the I/O port
*/
extern "C" void outb(unsigned short port, unsigned char data);

};

#endif // IO_IO_H
