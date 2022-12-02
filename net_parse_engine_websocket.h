#ifndef net_parse_engine_websocket_h__
#define net_parse_engine_websocket_h__

#include "libxnet.h"
#include "net_parse_engine.h"

class NetParseEngineWebsocket : public NetParseEngine {
	enum {
		WS_NULL,
		WS_WAITING_HANDSHAKE,
		WS_OPENED_READHEAD,
		WS_OPENED_READDATA,
		WS_ERROR
	} state;

	uint8_t* _recvBuffer;
	size_t _headerBufSize;
public:
	NetParseEngineWebsocket();
	~NetParseEngineWebsocket();
	virtual bool procRecv(NetConnection*conn, NetBuffer& recvBuffer);
	virtual void onConnCreate(NetConnection*conn);
	virtual bool write(NetConnection* conn, NetBuffer& sendBuffer, void* data, size_t size);
	
private:
	int _procHandshake(NetBuffer& recvBuffer, char *outBuf, size_t *outSize);
};

#endif // net_parse_engine_websocket_h__
