#ifndef net_log_h__
#define net_log_h__
#include <stdarg.h>

#define net_log_error(formatTxt, ...) log(LOG_ERROR, "file: [%s] line: [%d]: "formatTxt, __FILE__, __LINE__, __VA_ARGS__)

enum LogType
{
	LOG_INFO,
	LOG_ERROR
};

void log(LogType type, const char* formatTxt, ...);

#endif // net_log_h__
