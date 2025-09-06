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
    m_fileSystem = filesystem::resolve(this);
    if (!m_fileSystem)
        log::error("Failed to resolve filesystem for disk '%c'.\n", driveLetter);
}

Disk::~Disk()
{
    // TODO
    // delete m_fileSystemData;
}

disk::Type Disk::type() const
{
    return m_type;
}

uint8_t Disk::driveLetter() const
{
    return m_driveLetter;
}

unsigned int Disk::sectorSize() const
{
    return m_sectorSize;
}

filesystem::IFileSystem *Disk::fileSystem() const
{
    return m_fileSystem;
}

void *Disk::fileSystemData() const
{
    return m_fileSystemData;
}

disk::IDriver *Disk::driver() const
{
    return m_driver;
}

void Disk::setFileSystemData(void *fileSystemData)
{
    m_fileSystemData = fileSystemData;
}

size_t Disk::sectorsToBytes(size_t sectors) const
{
    return sectors * m_sectorSize;
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
