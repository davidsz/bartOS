#ifndef CORE_SERIALLOGGER_H
#define CORE_SERIALLOGGER_H

#include "log.h"

namespace core {

class SerialLogger : public log::ILogger {
public:
	void Info(const char *msg, VA_LIST) override;
	void Warning(const char *msg, VA_LIST) override;
	void Error(const char *msg, VA_LIST) override;
};

}; // namespace core

#endif // CORE_SERIALLOGGER_H
