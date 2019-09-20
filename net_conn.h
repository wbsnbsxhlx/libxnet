#ifndef net_conn_h__
#define net_conn_h__
#include "libxnet.h"
#include "net_buffer.h"
#include <windows.h>

class Network;
class NetConnection
{
public:
	static NetConnection* create(SOCKET so, const char* ip, unsigned short port);

	NetConnection();
	~NetConnection();

	void retain();
	void release();

	bool init(SOCKET so, const char* ip, unsigned short port);
	bool setNetwork(Network* network);

	bool write(void* data, size_t size);
	bool initBufSize(int _sendBufSize, int _recvBufSize);
	bool setConnId(net_conn_id_t connId);

	bool hasSendData(){ return _sendBuffer.empty(); }

	net_conn_id_t getConnId(){ return _connId; }

	SOCKET getSocket(){ return _socket; }

	bool send();
	void recv();

	void pushMsg();

	void close();
	void shutdown();
private:
	net_conn_id_t _connId;
	char _ip[16];
	unsigned short _port;
	SOCKET _socket;

	Network* _network;

	NetBuffer _sendBuffer;
	NetBuffer _recvBuffer;

	int _refCount;
};

#endif // net_conn_h__