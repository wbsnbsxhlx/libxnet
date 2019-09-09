#include "net_log.h"
#include <stdio.h>
#include <fstream>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS

const uint32_t LOG_LENGTH_MAX = 1024;

void log(LogType type, const char* formatTxt, ...)
{
	va_list argList;
	va_start(argList, formatTxt);

	char buf[LOG_LENGTH_MAX];

	char info[6] = "Info";
	if (type == ERROR)
	{
		strcpy_s(info, "Error");
	}

	sprintf_s(buf, "%s:%d %s:", __FILE__, __LINE__, info);
	vsprintf_s(buf+strlen(buf),LOG_LENGTH_MAX-strlen(buf), formatTxt, argList);
	
	
	std::ofstream ofs;
	ofs.open("net_log.log");

	ofs << buf << std::endl;
	ofs.close();
}
