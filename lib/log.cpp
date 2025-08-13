#include "log.h"
#include "vargs.h"

static log::ILogger *s_logger = nullptr;

namespace log {

void set_logger(ILogger *logger)
{
    s_logger = logger;
}

void info(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    if (s_logger)
        s_logger->Info(msg, args);
    VA_END(args);
}

void warning(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    if (s_logger)
        s_logger->Warning(msg, args);
    VA_END(args);
}

void error(const char *msg, ...)
{
    VA_LIST args;
    VA_START(args, msg);
    if (s_logger)
        s_logger->Error(msg, args);
    VA_END(args);
}

}; // namespace log
