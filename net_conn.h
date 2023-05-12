#ifndef net_conn_h__
#define net_conn_h__
#include "libxnet.h"
#include "net_buffer.h"
#include "net_network.h"
#include <mutex>

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

	NetConnection* nextConn;

	virtual bool init() { return true; }
	bool initNetwork(Network* network, SOCKET so, const char* ip, unsigned short port);
	bool setNetwork(Network* network);
	Network* getNetwork() { return _network; }

	bool write(void* data, size_t size);
	virtual bool initBufSize(size_t sendBufSize, size_t recvBufSize);
	bool setConnId(net_conn_id_t connId);

	bool hasSendData(){ return _sendBuffer.empty(); }

	net_conn_id_t getConnId(){ return _connId; }

	SOCKET getSocket() { return _socket; }

	void getConnInfo(char* ip, uint16_t* port);

	bool send();
	void recv();

	bool procRecv();

	void close();
	void shutdown();

	void recvedLength(size_t len);
	void sendedLength(size_t len);

	inline void setCloseFlag(bool flag) {
		_closeFlag = flag;
	}

	inline void setSendingFlag(bool flag) {
		_isSending = flag;
	}

	virtual void onConnCreate() = 0;
private:
	net_conn_id_t _connId;
	char _ip[16];
	unsigned short _port;
	SOCKET _socket;

	bool _closeFlag;
	bool _isSending;

	Network* _network;
	std::mutex _shutdownLock;

protected:
	virtual bool onProcRecv() = 0;
	virtual bool onWrite(void* data, size_t size) = 0;
	NetBuffer _sendBuffer;
	std::mutex _sendBufLock;
	NetBuffer _recvBuffer;
	std::mutex _recvBufLock;

public:
	NetConnectionOverlapped* sender;
	NetConnectionOverlapped* recver;
};

#endif // net_conn_h__