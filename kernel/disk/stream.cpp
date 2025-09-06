#include "stream.h"
#include "config.h"
#include "driver.h"
#include "log.h"

namespace disk {

Stream::Stream(disk::IDriver *driver)
    : m_driver(driver)
{
}

void Stream::Read(uint8_t *out, size_t bytes)
{
    // TODO: Eliminate the recursive implementation
    int lba = m_pos / SECTOR_SIZE;
    int offset = m_pos % SECTOR_SIZE;
    uint8_t buf[SECTOR_SIZE];

    m_driver->ReadSector(lba, 1, buf);

    int total_to_read = bytes > SECTOR_SIZE ? SECTOR_SIZE : bytes;
    for (int i = 0; i < total_to_read; i++)
        *out++ = buf[offset+i];

    // Adjust the stream
    m_pos += total_to_read;
    if (bytes > SECTOR_SIZE)
        Read(out, bytes - SECTOR_SIZE);
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
