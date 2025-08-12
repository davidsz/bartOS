#include "seriallogger.h"
#include "output/serial.h"
#include "vargs.h"

namespace core {

void SerialLogger::Info(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    serial::write("[INFO] ");
    serial::write(msg, args);
    VA_END(args);
}

void SerialLogger::Warning(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    serial::write("[WARNING] ");
    serial::write(msg, args);
    VA_END(args);
}

void SerialLogger::Error(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    serial::write("[ERROR] ");
    serial::write(msg, args);
    VA_END(args);
}

}; // namespace core
