#include "path.h"
#include "log.h"
#include "memory.h"
#include "status.h"

Path::Path(const char *path)
    : m_data(path)
{
    if (Parse() != Status::ALL_OK)
        log::error("Failed to parse path: %s\n", path);
}

int Path::Parse()
{
    size_t length = m_data.length();
    if (length < 3 || !isdigit(m_data[0]) || memcmp((void *)&m_data[1], ":/", 2) != 0)
        return Status::EBADPATH;

    m_parts = m_data.split('/');
    m_driveLetter = m_parts[0][0];
    return Status::ALL_OK;
}

const String &Path::Data() const
{
    return m_data;
}

char Path::DriveLetter() const
{
    return m_driveLetter;
}

const Vector<String> &Path::Components() const
{
    return m_parts;
}

String &Path::operator[](int index)
{
    return m_parts[index];
}

const String &Path::operator[](int index) const
{
    return m_parts[index];
}
