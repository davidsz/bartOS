#include "disk.h"
#include "driver.h"
#include "filesystem.h"
#include "list.h"
#include "log.h"

namespace disk {

static List<Disk *> s_disks;

Disk::Disk(disk::Type type, uint8_t driveLetter, IDriver *driver)
    : m_type(type)
    , m_driveLetter(driveLetter)
    , m_driver(driver)
{
    m_filesystem = filesystem::resolve(this);
    if (!m_filesystem)
        log::error("Failed to resolve filesystem for disk\n");
}

Disk::~Disk() = default;

disk::Type Disk::type()
{
    return m_type;
}

uint8_t Disk::driveLetter()
{
    return m_driveLetter;
}

unsigned int Disk::sectorSize()
{
    return m_sectorSize;
}

filesystem::IFileSystem *Disk::fileSystem()
{
    return m_filesystem;
}

disk::IDriver *Disk::driver()
{
    return m_driver;
}

void search_and_init_all()
{
    // TODO: Implement discovery logic
    // - Identify the available disks and their types (ATA or ATAPI)
    // - Assign a drive letter to each disk
    s_disks.push_back(new Disk(disk::PrimaryMaster, 0, new disk::ATADriver));
}

disk::Disk *get_by_letter(uint8_t letter)
{
    for (auto it = s_disks.begin(); it != s_disks.end(); it++) {
        if ((*it)->driveLetter() == letter)
            return *it;
    }
    return nullptr;
}

}; // namespace disk
