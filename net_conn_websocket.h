#ifndef net_conn_websocket_h__
#define net_conn_websocket_h__

#include "net_conn.h"

class NetConnectionWebsocket :public NetConnection {
	enum {
		WS_NULL,
		WS_WAITING_HANDSHAKE,
		WS_FRAME_HEAD,
		WS_FRAME_DATA,
		WS_ERROR
	} state;
public:
	union FrameHeader {
		struct {
			uint16_t opcode : 4;
			uint16_t rsv3 : 1;
			uint16_t rsv2 : 1;
			uint16_t rsv1 : 1;
			uint16_t fin : 1;
			uint16_t payloadLen : 7;
			uint16_t mask : 1;
			uint16_t payloadLen16;
		};

		struct {
			uint16_t _wordHead;
			uint16_t _wordExt;
		};
	};

	NetConnectionWebsocket();
	~NetConnectionWebsocket();
	virtual bool init();
	virtual bool initBufSize(size_t sendBufSize, size_t recvBufSize);
	virtual bool onProcRecv();
	virtual void onConnCreate();
	virtual bool onWrite(void* data, size_t size);

private:
	FrameHeader _frameHeader;
	uint32_t _dataSize;
	uint32_t _mask;
	uint8_t* _dataBuffer;
	uint32_t _bufSize;

	int _procHandshake(char *outBuf, size_t *outSize);
	int _procFrameHead();
	int _procFrameData();
};

#endif // net_conn_websocket_h__