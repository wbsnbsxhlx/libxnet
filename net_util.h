#ifndef net_util_h__
#define net_util_h__

#include "libxnet.h"

uint64_t get_cur_tick() {
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return li.QuadPart / 10000;
}

#endif // net_util_h__
