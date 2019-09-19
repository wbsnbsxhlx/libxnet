#ifndef net_conn_pool_h__
#define net_conn_pool_h__

#include "libxnet.h"
#include "net_conn.h"
#include <map>
#include <queue>

class NetConnectionPool
{
public:
	NetConnectionPool();
	bool removeConn(net_conn_id_t connId);
	net_conn_id_t addConn(NetConnection* conn);

	NetConnection* getConn(net_conn_id_t id);

	bool init(int sendBufSize, int recvBufSize);

private:

	std::map<net_conn_id_t, NetConnection*> _connMap;
	std::queue<net_conn_id_t> _connIdFreeQueue;
	net_conn_id_t _connIdMax;
};

#endif // net_conn_pool_h__