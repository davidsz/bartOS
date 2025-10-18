#include "stream.h"
#include "config.h"
#include "driver.h"
#include "log.h"
#include "memory.h"

namespace disk {

Stream::Stream(disk::IDriver *driver)
    : m_driver(driver)
{
}

Stream::~Stream()
{
}

void Stream::Read(uint8_t *out, size_t bytes)
{
    while (bytes > 0) {
        size_t offset = m_pos % SECTOR_SIZE;
        size_t total_to_read = SECTOR_SIZE - offset;
        if (bytes < total_to_read)
            total_to_read = bytes;

        // Read a sector from the disk
        int lba = m_pos / SECTOR_SIZE;
        uint8_t buf[SECTOR_SIZE];
        m_driver->ReadSector(lba, 1, buf);

        // Copy the results into the out buffer
        memcpy(out, buf + offset, total_to_read);
        out += total_to_read;

        m_pos += total_to_read;
        bytes -= total_to_read;
    }
}

void Stream::Write(const uint8_t *, size_t)
{
	// TODO: Implement writing data to the disk
}

void Stream::Seek(size_t pos)
{
	m_pos = pos;
}

size_t Stream::Position()
{
	return m_pos;
}

void Stream::Close()
{
}

}; // namespace disk
