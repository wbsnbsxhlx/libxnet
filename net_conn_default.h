#ifndef net_conn_default_h__
#define net_conn_default_h__

#include "net_conn.h"

class NetConnectionDefault :public NetConnection {
	struct NetMsgHeader {
		uint16_t size;
	};

	virtual bool onProcRecv();
	virtual void onConnCreate();
	virtual bool onWrite(void* data, size_t size);
};

#endif // net_conn_default_h__