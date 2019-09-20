#ifndef net_network_h__
#define net_network_h__

#include <windows.h>
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

	static Network* create(int threadNum, int maxClient, int recvBufSize, int sendBufSize);

	bool init(int threadNum, int maxClient, int recvBufSize, int sendBufSize);
	bool listen(const char* local_addr, unsigned short port);
	void connect(const char* remote_addr, unsigned short port);

	void shutdown();
	void createConn(SOCKET so, const char* ip, unsigned short port);
	void removeConn(NetConnection* conn);

	NetConnectionPool* getConnPool(){
		return _connPool;
	}

private:
	int _workerNum;
	int _maxClient;
	int _recvBufSize;
	int _sendBufSize;

	NetConnectionPool* _connPool;

	NetThreadListener* _threadListener;
	NetThreadWroker** _threadWorkerList;
};

#endif // net_network_h__