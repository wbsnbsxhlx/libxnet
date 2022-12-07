#ifndef net_network_h__
#define net_network_h__

#include <thread>
#include "libxnet.h"
#include "net_msg_queue.h"
#include "net_conn_pool.h"

class NetThreadWroker;
class NetThreadListener;
class NetConnection;
class Network
{
public:
	Network(int mode);
	~Network();

	int NetMode;

	bool init(int threadNum, int maxClient, int recvBufSize, int sendBufSize);
	bool listen(const char* local_addr, unsigned short port);
	net_conn_id_t connect(const char* remote_addr, unsigned short port);

	void shutdown();
	NetConnection* createConn(SOCKET so, const char* ip, unsigned short port);
	void removeConn(net_conn_id_t connId);

	NetConnection* getConn(net_conn_id_t connId) { return _connPool->getConn(connId); }

private:
	int _workerNum;
	int _maxClient;
	int _recvBufSize;
	int _sendBufSize;

	NetConnectionPool* _connPool;

	NetThreadListener* _threadListener;
	NetThreadWroker** _threadWorkerList;

	NetMsgQueue msgQueue;

	HANDLE _iocp;

public:
	void freeMsg(net_msg_s& msg);
	void pushMsg(net_msg_s& msg);
	void pushMsg(int typ, net_conn_id_t conn_id, uint8_t* data, size_t size);
	bool popMsg(net_msg_s& msg);
};

#endif // net_network_h__