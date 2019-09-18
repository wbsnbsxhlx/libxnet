#include "net_conn_pool.h"
#include "net_log.h"
#include "windows.h"

NetConnectionPool::NetConnectionPool()
	:_connIdMax(1)
{

}
net_conn_id_t NetConnectionPool::newConn(SOCKET so, const char* ip, unsigned short port)
{
	NetConnection* conn = NetConnection::create(so, ip, port);
	conn->initBufSize(_sendBufSize, _recvBufSize);
	return _addConn(conn);
}

NetConnection* NetConnectionPool::getConn(net_conn_id_t connId)
{
	NetConnection* ret = nullptr;
	if (_connMap.find(connId) != _connMap.end()){
		ret = _connMap[connId];
	}

	return ret;
}

bool NetConnectionPool::init(int sendBufSize, int recvBufSize)
{
	_sendBufSize = sendBufSize;
	_recvBufSize = recvBufSize;
}

bool NetConnectionPool::removeConn(net_conn_id_t connId)
{
	if (connId == INVALID_CONN_ID || _connMap.find(connId) == _connMap.end()){
		log(LOG_ERROR, "remove error, connId:%d is not exsist!", connId);
		return false;
	}

	NetConnection* conn = _connMap[connId];
	_connMap.erase(connId);
	delete conn;

	return true;
}

net_conn_id_t NetConnectionPool::_addConn(NetConnection* conn)
{
	net_conn_id_t connId = INVALID_CONN_ID;

	if (_connIdFreeQueue.empty()){
		connId = _connIdMax++;
	}
	else {
		connId = _connIdFreeQueue.front();
		_connIdFreeQueue.pop();
	}

	if (_connMap.find(connId) != _connMap.end()){
		return INVALID_CONN_ID;
	}
	_connMap[connId] = conn;


	return connId;
}

