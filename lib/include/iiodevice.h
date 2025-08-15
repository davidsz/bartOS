#ifndef LIB_IIODEVICE_H
#define LIB_IIODEVICE_H

#include <stddef.h>
#include <stdint.h>

class IIODevice {
public:
	virtual void Read(uint8_t *buffer, size_t bytes) = 0;
	virtual void Write(const uint8_t *data, size_t bytes) = 0;
	virtual void Seek(size_t pos) = 0;
	virtual size_t Position() = 0;
	virtual void Close() = 0;
};

#endif // LIB_IIODEVICE_H
