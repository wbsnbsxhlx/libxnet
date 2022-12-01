#ifndef net_conn_h__
#define net_conn_h__
#include "libxnet.h"
#include "net_buffer.h"
#include "net_parse_engine_default.h"
#include "net_network.h"

class Network;
class NetConnection;
class NetConnectionOverlapped : public OVERLAPPED {
public:
	NetConnection* conn;
	bool isSender;
};

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	bool init(Network* network, SOCKET so, const char* ip, unsigned short port);
	bool setNetwork(Network* network);
	Network* getNetwork() { return _network; }

	bool write(void* data, size_t size);
	bool initBufSize(int _sendBufSize, int _recvBufSize);
	bool setConnId(net_conn_id_t connId);

	bool hasSendData(){ return _sendBuffer.empty(); }

	net_conn_id_t getConnId(){ return _connId; }

	SOCKET getSocket(){ return _socket; }

	bool send();
	void recv();

	void procRecv();

	void close();
	void shutdown();

	void recvedLength(size_t len);
	void sendedLength(size_t len);
private:
	net_conn_id_t _connId;
	char _ip[16];
	unsigned short _port;
	SOCKET _socket;

	Network* _network;

	NetBuffer _sendBuffer;
	NetBuffer _recvBuffer;
public:
	NetConnectionOverlapped* sender;
	NetConnectionOverlapped* recver;
	NetParseEngine* engine;
};

#endif // net_conn_h__