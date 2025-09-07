#include "path.h"
#include "log.h"
#include "memory.h"
#include "status.h"

Path::Path()
    : m_data()
    , m_valid(false)
{
}

Path::Path(const char *path)
    : m_data(path)
    , m_valid(false)
{
    if (Parse() != Status::ALL_OK)
        log::error("Failed to parse path: %s\n", path);
    else
        m_valid = true;
}

int Path::Parse()
{
    size_t length = m_data.length();
    if (length < 3 || !isdigit(m_data[0]) || memcmp((void *)&m_data[1], ":/", 2) != 0)
        return Status::E_BAD_PATH;

    m_parts = m_data.split('/');
    m_driveLetter = m_parts[0][0];
    return Status::ALL_OK;
}

const String &Path::data() const
{
    return m_data;
}

bool Path::valid() const
{
    return m_valid;
}

char Path::driveLetter() const
{
    return m_driveLetter;
}

const Vector<String> &Path::components() const
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
