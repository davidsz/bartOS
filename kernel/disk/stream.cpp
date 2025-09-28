#include "stream.h"
#include "config.h"
#include "driver.h"
#include "log.h"

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
    int lba = m_pos / SECTOR_SIZE;
    int offset = m_pos % SECTOR_SIZE;
    size_t total_to_read = bytes;
    bool overflow = (offset + total_to_read) >= SECTOR_SIZE;
    if (overflow)
        total_to_read -= (offset + total_to_read) - SECTOR_SIZE;

    uint8_t buf[SECTOR_SIZE];
    m_driver->ReadSector(lba, 1, buf);

    // Fill the output buffer
    for (size_t i = 0; i < total_to_read; i++)
        *out++ = buf[offset + i];

    // Adjust the stream
    m_pos += total_to_read;
    if (overflow)
        Read(out, bytes - total_to_read);
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
