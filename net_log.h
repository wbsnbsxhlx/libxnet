#ifndef net_log_h__
#define net_log_h__
#include <stdarg.h>

enum LogType
{
	INFO,
	ERROR
};

void log(LogType type, const char* formatTxt, ...);

#endif // net_log_h__