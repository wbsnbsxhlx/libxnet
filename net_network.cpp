#include "net_network.h"
#include "net_log.h"
#include "net_thread_listener.h"
#include "net_thread_worker.h"
#include "net_conn.h"
#include <thread>

Network::Network(int mode)
	:_workerNum(0),
	_maxClient(0),
	_recvBufSize(0),
	_sendBufSize(0),
	NetMode(mode),
	_connPool(nullptr),
	_threadListener(nullptr),
	_threadWorkerList(nullptr) {}

Network::~Network() {}

bool Network::init(int threadNum, int maxClient, int recvBufSize, int sendBufSize) {
	if (threadNum < 1) {
		log(LOG_ERROR, "[threadNum=%d] threadNum must >1!", threadNum);
	}
	if (maxClient < 1) {
		log(LOG_ERROR, "[maxClient=%d] maxClient must >1!", maxClient);
	}
	if (maxClient > 0x3fff) {
		log(LOG_ERROR, "[maxClient=%d] maxClient must < %d!", maxClient, 0x3fff);
	}
	if (recvBufSize < 64) {
		log(LOG_ERROR, "[recvBufSize=%d] recvBufSize must >64!", recvBufSize);
	}
	if (sendBufSize < 64) {
		log(LOG_ERROR, "[sendBufSize=%d] sendBufSize must >64!", sendBufSize);
	}

	_workerNum = threadNum;
	_maxClient = maxClient;
	_recvBufSize = recvBufSize;
	_sendBufSize = sendBufSize;

	_connPool = new NetConnectionPool(NetMode);
	_connPool->init(maxClient, sendBufSize, recvBufSize);

	_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	_threadWorkerList = new NetThreadWroker*[threadNum];
	for (int i = 0; i < threadNum; ++i) {
		_threadWorkerList[i] = new NetThreadWroker();
		_threadWorkerList[i]->start(_iocp);
	}

	return true;
}

bool Network::listen(const char* local_addr, unsigned short port) {
	if (_threadListener != nullptr) {
		log(LOG_ERROR, "_threadListener is exsist");
		return false;
	}

	_threadListener = new NetThreadListener();
	_threadListener->attachNetwork(this);
	return _threadListener->start(local_addr, port);
}

net_conn_id_t Network::connect(const char* remote_addr, unsigned short port) {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(remote_addr);
	address.sin_port = htons(port);

	SOCKET so = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (::connect(so, (sockaddr*)&address, sizeof(address)) < 0){
		closesocket(so);
		return INVALID_CONN_ID;
	}

	NetConnection* conn = createConn(so, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
	if ( conn == nullptr) {
		::shutdown(so, SD_BOTH);
		closesocket(so);
		return INVALID_CONN_ID;
	}

	return conn->getConnId();
}

void Network::shutdown() {
	if (_threadListener != nullptr) {
		_threadListener->stop();
		delete _threadListener;
	}

	for (int i = 0; i < _workerNum; ++i) {
		_threadWorkerList[i]->stop();
		delete _threadWorkerList[i];
	}
	delete[] _threadWorkerList;

	if (_connPool != nullptr) {
		_connPool->clear();
		delete _connPool;
	}
}


NetConnection* Network::createConn(SOCKET so, const char* ip, unsigned short port) {
	NetConnection *conn = _connPool->createConn();
	if (conn == nullptr) {
		return nullptr;
	}

	if (!conn->init() || !conn->initNetwork(this, so, ip, port)) {
		_connPool->removeConn(conn->getConnId());
		return nullptr;
	}

	conn->onConnCreate();

	CreateIoCompletionPort((HANDLE)so, _iocp, (DWORD)so, 0);
	conn->recv();

	return conn;
}

void Network::removeConn(net_conn_id_t connId) {
	NetConnection* conn = getConn(connId);
	if (conn != nullptr) {
		conn->shutdown();
	}

	if (_connPool->removeConn(connId)) {
		net_msg_s msg;
		msg.conn_id = connId;
		msg.type = NET_MSG_DISCONNECTED;
		msg.data = nullptr;
		msg.size = 0;
		pushMsg(msg);
	}
}

void Network::pushMsg(int typ, net_conn_id_t conn_id, uint8_t* data, size_t size) {
	net_msg_s msg;
	msg.conn_id = conn_id;
	msg.type = typ;
	if (data != nullptr && size != 0) {
		msg.data = new uint8_t[size];
		memcpy(msg.data, data, size);
		msg.size = size;
	} else {
		msg.data = nullptr;
		msg.size = 0;
	}

	msgQueue.pushMsg(msg);
}

void Network::pushMsg(net_msg_s& msg) {
	msgQueue.pushMsg(msg);
}

bool Network::popMsg(net_msg_s& msg) {
	return msgQueue.popMsg(msg);
}