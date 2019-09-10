#include "net_log.h"
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdint.h>

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
	vsprintf_s(buf+strlen(buf),LOG_LENGTH_MAX-strlen(buf)-1, formatTxt, argList);
	
	
	std::ofstream ofs;
	if (!ofs.is_open())
		return;

	ofs.open("net_log.log");

	ofs << buf << std::endl;
	ofs.close();
}
