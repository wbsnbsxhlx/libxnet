#include "net_conn_pool.h"
#include "net_log.h"
#include "libxnet.h"

NetConnectionPool::NetConnectionPool()
	:_connIdMax(1) {

}

bool NetConnectionPool::init(int maxClient, int sendBufSize, int recvBufSize) {
	_maxClient = maxClient;
	_sendBufSize = sendBufSize;
	_recvBufSize = recvBufSize;

	return true;
}


void NetConnectionPool::clear() {
	for (auto it = _connMap.begin(); it != _connMap.end(); ){
		it++->second->close();
	}
}

NetConnection* NetConnectionPool::_getFreeConn() {
	if (_connIdMax >= _maxClient) {
		return nullptr;
	}

	NetConnection *ret = nullptr;
	if (_freeConnVec.size() == 0) {
		ret = new NetConnection();
		ret->initBufSize(_sendBufSize, _recvBufSize);
		ret->setConnId(_connIdMax++);
	} else {
		ret = _freeConnVec.back();
		_freeConnVec.pop_back();
	}

	_connMap[ret->getConnId()] = ret;

	return ret;
}
void NetConnectionPool::_freeConn(NetConnection* conn) {
	_connMap.erase(conn->getConnId());
	_freeConnVec.push_back(conn);
}

NetConnection* NetConnectionPool::createConn(SOCKET so, const char* ip, unsigned short port) {
	std::lock_guard<std::mutex> l(_connmapLock);

	NetConnection* ret = _getFreeConn();
	if (ret == nullptr) {
		return nullptr;
	}

	if (!ret->init(so, ip, port)) {
		_freeConn(ret);
		ret = nullptr;
	}
	return ret;
}

bool NetConnectionPool::removeConn(net_conn_id_t connId) {
	if (connId == INVALID_CONN_ID || _connMap.find(connId) == _connMap.end()) {
		log(LOG_ERROR, "remove error, connId:%d is not exsist!", connId);
		return false;
	}

	std::lock_guard<std::mutex> l(_connmapLock);
	NetConnection* conn = _connMap[connId];
	_freeConn(conn);

	return true;
}

NetConnection* NetConnectionPool::getConn(net_conn_id_t connId) {
	NetConnection* ret = nullptr;
	if (_connMap.find(connId) != _connMap.end()) {
		ret = _connMap[connId];
	}

	return ret;
}