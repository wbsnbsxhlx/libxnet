#include "net_conn_pool.h"
#include "net_conn.h"
#include "net_conn_default.h"
#include "net_conn_websocket.h"
#include "net_log.h"
#include "libxnet.h"

NetConnectionPool::NetConnectionPool(int netmode)
	:_connIdMax(1),
	_netMode(netmode),
	_freeConnList(nullptr){

}

bool NetConnectionPool::init(int maxClient, int sendBufSize, int recvBufSize) {
	_maxClient = maxClient;
	_sendBufSize = sendBufSize;
	_recvBufSize = recvBufSize;

	return true;
}


void NetConnectionPool::clear() {
	for (auto it = _connMap.begin(); it != _connMap.end();) {
		it++->second->close();
	}
}

NetConnection* NetConnectionPool::_getFreeConn() {
	NetConnection *ret = _freeConnList;
	if (ret == nullptr) {
		if (_connIdMax >= _maxClient) {
			return nullptr;
		}
		if (_netMode == NET_MODE_DEFAULT) {
			ret = new NetConnectionDefault();
		} else if (NET_MODE_WEBSOCKET) {
			ret = new NetConnectionWebsocket();
		}
		ret->initBufSize(_sendBufSize, _recvBufSize);
		ret->setConnId(_connIdMax++);
	} else {
		_freeConnList = ret->nextConn;
	}

	_connMap[ret->getConnId()] = ret;

	return ret;
}
void NetConnectionPool::_freeConn(NetConnection* conn) {
	_connMap.erase(conn->getConnId());

	conn->nextConn = _freeConnList;
	_freeConnList = conn;
}

NetConnection* NetConnectionPool::createConn() {
	std::lock_guard<std::mutex> l(_connmapLock);
	NetConnection* ret = _getFreeConn();
	if (ret == nullptr) {
		return nullptr;
	}

	return ret;
}

bool NetConnectionPool::removeConn(net_conn_id_t connId) {
	std::lock_guard<std::mutex> l(_connmapLock);
	if (connId == INVALID_CONN_ID || _connMap.find(connId) == _connMap.end()) {
		log(LOG_ERROR, "remove error, connId:%d is not exsist!", connId);
		return false;
	}

	NetConnection* conn = _connMap[connId];
	_freeConn(conn);

	return true;
}

NetConnection* NetConnectionPool::getConn(net_conn_id_t connId) {
	std::lock_guard<std::mutex> l(_connmapLock);
	if (_connMap.find(connId) != _connMap.end()) {
		return _connMap[connId];
	} else {
		return nullptr;
	}
}