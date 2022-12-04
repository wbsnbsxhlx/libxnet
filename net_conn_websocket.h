#ifndef net_conn_websocket_h__
#define net_conn_websocket_h__

#include "net_conn.h"

class NetConnectionWebsocket :public NetConnection {
	enum {
		WS_NULL,
		WS_WAITING_HANDSHAKE,
		WS_OPENED_READHEAD,
		WS_OPENED_READDATA,
		WS_ERROR
	} state;

public:
	NetConnectionWebsocket();
	~NetConnectionWebsocket();
	virtual bool onProcRecv();
	virtual void onConnCreate();
	virtual bool onWrite(void* data, size_t size);
};

#endif // net_conn_websocket_h__