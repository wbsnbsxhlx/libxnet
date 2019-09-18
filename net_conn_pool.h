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
	net_conn_id_t newConn(SOCKET so, const char* ip, unsigned short port);

	NetConnection* getConn(net_conn_id_t id);

	bool init(int sendBufSize, int recvBufSize);

private:
	net_conn_id_t _addConn(NetConnection* conn);

	std::map<net_conn_id_t, NetConnection*> _connMap;
	std::queue<net_conn_id_t> _connIdFreeQueue;
	net_conn_id_t _connIdMax;

	int _recvBufSize;
	int _sendBufSize;
};

#endif // net_conn_pool_h__