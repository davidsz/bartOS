#ifndef LIB_LOG_H
#define LIB_LOG_H

#include "vargs.h"

namespace log {

class ILogger {
public:
	virtual void Info(const char *msg, VA_LIST) = 0;
	virtual void Warning(const char *msg, VA_LIST) = 0;
	virtual void Error(const char *msg, VA_LIST) = 0;
};

void set_logger(ILogger *logger);

void info(const char *msg, ...);
void warning(const char *msg, ...);
void error(const char *msg, ...);

};

#endif // LIB_LOG_H
