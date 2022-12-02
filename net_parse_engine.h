#ifndef net_parse_engine_h__
#define net_parse_engine_h__

#include "libxnet.h"
#include "net_buffer.h"

class NetConnection;
class NetParseEngine {
public:
	virtual bool procRecv(NetConnection*conn, NetBuffer& recvBuffer) = 0;
	virtual void onConnCreate(NetConnection*conn) = 0;
	virtual bool write(NetConnection* conn, NetBuffer& sendBuffer, void* data, size_t size) = 0;
};

#endif // net_parse_engine_h__
