#ifndef net_parse_engine_default_h__
#define net_parse_engine_default_h__

#include "libxnet.h"
#include "net_parse_engine.h"

class NetParseEngineDefault : public NetParseEngine{
	struct NetMsgHeader {
		uint32_t size;
	};

public:
	virtual bool procRecv(NetConnection*conn, NetBuffer& recvBuffer);
	virtual void onConnCreate(NetConnection*conn);
	virtual bool write(NetConnection* conn, NetBuffer& sendBuffer, void* data, size_t size);
};

#endif // net_parse_engine_default_h__
