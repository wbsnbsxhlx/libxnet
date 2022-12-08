#ifndef net_conn_pool_h__
#define net_conn_pool_h__

#include "libxnet.h"
#include <map>
#include <queue>
#include <mutex>

class NetConnection;
class NetConnectionPool
{
public:
	NetConnectionPool(int netmode);
	NetConnection* createConn();
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

	int _netMode;

	std::map<net_conn_id_t, NetConnection*> _connMap;
	NetConnection* _freeConnList;
	net_conn_id_t _connIdMax;

	std::mutex _connmapLock;
};

#endif // net_conn_pool_h__