#ifndef net_conn_h__
#define net_conn_h__
#include "libxnet.h"

#include <windows.h>
class NetConnection
{
public:
	static NetConnection* create(SOCKET so, const char* ip, unsigned short port);

	NetConnection();
	bool init(SOCKET so, const char* ip, unsigned short port);

	bool send(void* data, size_t size);
	
private:
	char _ip[16];
	unsigned short _port;
	SOCKET _socket;
};

#endif // net_conn_h__