#ifndef DISK_STREAM_H
#define DISK_STREAM_H

#include "iiodevice.h"

// Byte-position abstraction above the disk driver

namespace disk {

class Stream : public IIODevice {
public:
	void Read(uint8_t *out, size_t bytes) override;
	void Write(const uint8_t *data, size_t bytes) override;
	void Seek(size_t pos) override;
	size_t Position() override;
	void Close() override;

private:
    size_t m_pos = 0;
};

}; // namespace disk

#endif // DISK_STREAM_H
