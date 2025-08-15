#ifndef DISK_DRIVER_H
#define DISK_DRIVER_H

// The lowest level implementation of disk handling:
// reading sectors using Logical Block Addressing (LBA).

/*
 * TODO: It only works for HDD.
 * - Identify the available disks and their types (ATA or ATAPI)
 * - Add support for multiple disks, don't default to the master one
 * - Create a disk identifier type (protocol type, block size, max size, etc.)
 */

namespace disk {

void read_sector(int lba, int total, void *buffer);

}; // namespace disk

#endif // DISK_DRIVER_H
