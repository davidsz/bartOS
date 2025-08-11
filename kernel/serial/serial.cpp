#include "serial.h"
#include "status.h"
#include "core/io.h"

#define PORT 0x3f8    // COM1 (0x3f8)

// It's a blocking, but really simple implementation
static void serial_wait_for_transmit() {
    while (!(core::inb(PORT + 5) & 0x20));
}

static void serial_write_char(char c) {
    serial_wait_for_transmit();
    core::outb(PORT, c);
}

namespace serial {

int init()
{
    core::outb(PORT + 1, 0x00);    // Disable all interrupts
    core::outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    core::outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    core::outb(PORT + 1, 0x00);    //                  (hi byte)
    core::outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    core::outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    core::outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

    // TODO: Maybe move this out to a dedicated test when we have a test framework
    core::outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    core::outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    // Check if serial is faulty (i.e: not same byte as sent)
    if(core::inb(PORT + 0) != 0xAE)
       return Status::EIO;

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    core::outb(PORT + 4, 0x0F);
    return Status::ALL_OK;
}

void write(const char *msg)
{
    while (*msg)
        serial_write_char(*msg++);
}

}; // namespace serial
