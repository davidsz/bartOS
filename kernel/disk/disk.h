#ifndef DISK_DISK_H
#define DISK_DISK_H

#include <stdint.h>

namespace filesystem {
class IFileSystem;
};

namespace disk {

enum Type {
    PrimaryMaster = 0,
    // PrimarySlave = 1,
    // SecondaryMaster = 2,
    // SecondarySlave = 3,
};

class IDriver;

class Disk {
public:
    Disk(disk::Type type, uint8_t driveLetter, IDriver *driver);
    ~Disk();

    Disk(const Disk &) = delete;
    Disk &operator=(const Disk &) = delete;

    disk::Type type();
    uint8_t driveLetter();
    unsigned int sectorSize();
    filesystem::IFileSystem *fileSystem();
    disk::IDriver *driver();

private:
    disk::Type m_type;
    uint8_t m_driveLetter;
    unsigned int m_sectorSize = 512;
    filesystem::IFileSystem *m_filesystem;
    disk::IDriver *m_driver;
};

void search_and_init_all();
Disk *get_by_letter(uint8_t letter);

}; // namespace disk

#endif // DISK_DISK_H
