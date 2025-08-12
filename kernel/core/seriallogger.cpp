#include "seriallogger.h"
#include "output/serial.h"
#include "vargs.h"

namespace core {

void SerialLogger::Info(const char *msg, VA_LIST args)
{
    serial::write("[INFO] ");
    serial::write(msg, args);
}

void SerialLogger::Warning(const char *msg, VA_LIST args)
{
    serial::write("[WARNING] ");
    serial::write(msg, args);
}

void SerialLogger::Error(const char *msg, VA_LIST args)
{
    serial::write("[ERROR] ");
    serial::write(msg, args);
}

}; // namespace core
