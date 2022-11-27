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

net_conn_id_t gen_conn_id(uint32_t idx) {
	uint64_t tick = get_cur_tick();
	return (net_conn_id_t)(((tick & 0x3ffff) << 14) | (idx & 0x3fff));
}

uint32_t get_conn_id_idx(net_conn_id_t connId) {
	return connId & 0x3fff;
}

#endif // net_util_h__
