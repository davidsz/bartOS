#include "serial.h"
#include "core/io.h"
#include "formatdevice.h"
#include "status.h"

#define PORT 0x3f8    // COM1

class SerialPort : public core::FormatDevice
{
public:
    // core::FormatDevice
    void PutChar(char c) override;
    void FormatProcessed() override {}

    int Init();
private:
    void WaitForTransmit();
};

static SerialPort s_serialPort;

void SerialPort::PutChar(char c)
{
    // It's a blocking, but really simple implementation
    WaitForTransmit();
    core::outb(PORT, c);
}

void SerialPort::WaitForTransmit()
{
    while (!(core::inb(PORT + 5) & 0x20));
}

int SerialPort::Init()
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

namespace serial {

int init()
{
    return s_serialPort.Init();
}

void write(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    write(msg, args);
    VA_END(args);
}

void write(const char *msg, VA_LIST args)
{
    s_serialPort.ProcessFormat(msg, args);
}

}; // namespace serial
