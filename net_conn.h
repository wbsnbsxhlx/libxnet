#ifndef net_conn_h__
#define net_conn_h__
#include "libxnet.h"
#include "net_buffer.h"
#include <windows.h>

class NetConnection
{
public:
	static NetConnection* create(SOCKET so, const char* ip, unsigned short port);

	NetConnection();
	bool init(SOCKET so, const char* ip, unsigned short port);

	bool send();
	bool write(void* data, size_t size);
	bool initBufSize(int _sendBufSize, int _recvBufSize);

private:
	char _ip[16];
	unsigned short _port;
	SOCKET _socket;

	NetBuffer _sendBuffer;
	NetBuffer _recvBuffer;
};

#endif // net_conn_h__