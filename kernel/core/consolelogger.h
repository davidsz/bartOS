#ifndef CORE_CONSOLELOGGER_H
#define CORE_CONSOLELOGGER_H

#include "log.h"

namespace core {

class ConsoleLogger : public log::ILogger {
public:
	void Info(const char *msg, VA_LIST) override;
	void Warning(const char *msg, VA_LIST) override;
	void Error(const char *msg, VA_LIST) override;
};

}; // namespace core

#endif // CORE_CONSOLELOGGER_H
