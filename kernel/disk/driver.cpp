#include "driver.h"
#include "core/io.h"

#include "log.h"

namespace disk {

void read_sector(int lba, int total, void *buffer)
{
    log::info("read_sector(%d, %d, %p)\n", lba, total, buffer);
    // Device/Head register (port 0x1F6)
    // 7   6   5   4   3   2   1   0
    // 1   L   1   D   H   H   H   H
    // -----------------------------
    // 1 = Reserved bits
    // L = LBA mode enabled
    // D = Device (master is 0, slave is 1)
    // H = highest 4 bits of the LBA
    // -----------------------------
    core::outb(0x1F6, (lba >> 24) | 0b11100000);
    // Sector Count register (port 0x1F2)
    core::outb(0x1F2, total);
    // LBA Low, bits 0-7 (port 0x1F3)
    core::outb(0x1F3, (unsigned char)(lba & 0xff));
    // LBA Mid, bits 8-15 (port 0x1F4)
    core::outb(0x1F4, (unsigned char)(lba >> 8));
    // LBA High, bits 16-23 (port 0x1F5)
    core::outb(0x1F5, (unsigned char)(lba >> 16));
    // Command/Status register (port 0x1F7)
    // Action: Reading (0x20) or writing (0x30) sectors
    core::outb(0x1F7, 0x20);

    unsigned short *ptr = (unsigned short *)buffer;
    for (int b = 0; b < total; b++) {
        log::info("Waiting for BSY = 0...\n");
        while (core::inb(0x1F7) & 0x80);

        log::info("Waiting for DRDY = 1...\n");
        while (!(core::inb(0x1F7) & 0x40));

        // Check the ERR bit
        if (core::inb(0x1F7) & 0x01) {
            uint8_t error = core::inb(0x1F1);
            log::error("ATA drive error: %02x\n", error);
            return;
        }

        // Wait for the buffer to be ready:
        // reading status from the Command/Status register
        // and test the Data Request Ready (DRQ) bit
        log::info("Waiting for DRQ...\n");
        while (!(core::inb(0x1F7) & 0b00001000));

        // Copy from hard disk to memory
        // by reading words the Data register (port 0x1F0)
        for (int i = 0; i < 256; i++, ptr++)
            *ptr = core::inw(0x1F0);
    }
}

}; // namespace disk
