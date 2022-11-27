#ifndef net_conn_pool_h__
#define net_conn_pool_h__

#include "libxnet.h"
#include "net_conn.h"
#include <map>
#include <queue>
#include <mutex>

class NetConnectionPool
{
public:
	NetConnectionPool();
	NetConnection* createConn(SOCKET so, const char* ip, unsigned short port);
	bool removeConn(net_conn_id_t connId);

	NetConnection* getConn(net_conn_id_t id);

	bool init(int maxClient, int sendBufSize, int recvBufSize);

	void clear();

private:
	NetConnection* _getFreeConn();
	void _freeConn(NetConnection* conn);

	int _maxClient;
	int _recvBufSize;
	int _sendBufSize;

	std::map<net_conn_id_t, NetConnection*> _connMap;
	std::vector<NetConnection*> _freeConnVec;
	net_conn_id_t _connIdMax;

	std::mutex _connmapLock;
};

#endif // net_conn_pool_h__