#include "consolelogger.h"
#include "output/console.h"
#include "vargs.h"

namespace core {

void ConsoleLogger::Info(const char *msg, VA_LIST args)
{
    console::print("[INFO] ");
    console::print(msg, args);
}

void ConsoleLogger::Warning(const char *msg, VA_LIST args)
{
    console::print("[WARNING] ");
    console::print(msg, args);
}

void ConsoleLogger::Error(const char *msg, VA_LIST args)
{
    console::print("[ERROR] ");
    console::print(msg, args);
}

}; // namespace core
