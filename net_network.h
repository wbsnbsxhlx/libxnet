#ifndef net_network_h__
#define net_network_h__

#include <thread>
#include "libxnet.h"
#include "net_conn_pool.h"

class NetThreadWroker;
class NetThreadListener;
class NetConnection;
class Network
{
public:
	Network();
	~Network();

	bool init(int threadNum, int maxClient, int recvBufSize, int sendBufSize);
	bool listen(const char* local_addr, unsigned short port);
	void connect(const char* remote_addr, unsigned short port);

	void shutdown();
	NetConnection* createConn(SOCKET so, const char* ip, unsigned short port);
	void removeConn(NetConnection* conn);

	NetConnection* getConn(net_conn_id_t connId) { return _connPool->getConn(connId); }

private:
	int _workerNum;
	int _maxClient;
	int _recvBufSize;
	int _sendBufSize;

	NetConnectionPool* _connPool;

	NetThreadListener* _threadListener;
	NetThreadWroker** _threadWorkerList;

	std::queue<net_msg_s> _queueMsgs;
	std::mutex _msgQueueLock;
	void _clearQueueMsgs();

	HANDLE _iocp;
public:
	void freeMsg(net_msg_s& msg);
	void pushMsg(net_msg_s& msg);
	bool popMsg(net_msg_s& msg);
};

#endif // net_network_h__