#ifndef DISK_DRIVER_H
#define DISK_DRIVER_H

// The lowest level implementation of disk handling:
// reading sectors using Logical Block Addressing (LBA).

namespace disk {

class IDriver {
public:
    // TODO: Pass the disk::Type here and don't default to the primary master one
    virtual void ReadSector(int lba, int total, void *buffer) = 0;
};

class ATADriver : public IDriver {
public:
    void ReadSector(int lba, int total, void *buffer) override;
};

}; // namespace disk

#endif // DISK_DRIVER_H
