#include "net_conn_pool.h"
#include "net_log.h"

NetConnectionPool::NetConnectionPool()
	:_connIdMax(1)
{

}

net_conn_id_t NetConnectionPool::addConn(NetConnection* conn)
{ 
	net_conn_id_t connId = INVALID_CONN_ID;
	

	if (_connIdFreeQueue.empty()){
		connId = _connIdMax++;
	}else {
		connId = _connIdFreeQueue.front();
		_connIdFreeQueue.pop();
	}

	if (_connMap.find(connId) != _connMap.end()){
		return INVALID_CONN_ID;
	}
	_connMap[connId] = conn;


	return connId;
}

bool NetConnectionPool::removeConn(net_conn_id_t connId)
{
	if (connId == INVALID_CONN_ID || _connMap.find(connId) == _connMap.end()){
		log(LOG_ERROR, "remove error, connId:%d is not exsist!", connId);
		return false;
	}
	_connMap.erase(connId);
	return true;
}

