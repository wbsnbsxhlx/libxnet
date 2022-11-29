#include "net_network.h"
#include "net_log.h"
#include "net_thread_listener.h"
#include "net_thread_worker.h"
#include <thread>

Network::Network()
	:_workerNum(0),
	_maxClient(0),
	_recvBufSize(0),
	_sendBufSize(0),
	_connPool(nullptr),
	_threadListener(nullptr),
	_threadWorkerList(nullptr)
{
}

Network::~Network()
{
	_clearQueueMsgs();
}

bool Network::init(int threadNum, int maxClient, int recvBufSize, int sendBufSize)
{
	if (threadNum < 1){
		log(LOG_ERROR, "[threadNum=%d] threadNum must >1!", threadNum);
	}
	if (maxClient < 1){
		log(LOG_ERROR, "[maxClient=%d] maxClient must >1!", maxClient);
	}
	if (maxClient > 0x3fff) {
		log(LOG_ERROR, "[maxClient=%d] maxClient must < %d!", maxClient, 0x3fff);
	}
	if (recvBufSize < 64){
		log(LOG_ERROR, "[recvBufSize=%d] recvBufSize must >64!", recvBufSize);
	}
	if (sendBufSize < 64){
		log(LOG_ERROR, "[sendBufSize=%d] sendBufSize must >64!", sendBufSize);
	}

	_workerNum = threadNum;
	_maxClient = maxClient;
	_recvBufSize = recvBufSize;
	_sendBufSize = sendBufSize;

	_connPool = new NetConnectionPool();
	_connPool->init(maxClient, sendBufSize, recvBufSize);

	_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	_threadWorkerList = new NetThreadWroker*[threadNum];
	for (int i = 0; i < threadNum; ++i){
		_threadWorkerList[i] = new NetThreadWroker();
		_threadWorkerList[i]->start(_iocp);
	}

	return true;
}

bool Network::listen(const char* local_addr, unsigned short port)
{
	if (_threadListener != nullptr) {
		log(LOG_ERROR, "_threadListener is exsist");
		return false;
	}

	_threadListener = new NetThreadListener();
	_threadListener->attachNetwork(this);
	return _threadListener->start(local_addr, port);
}

void Network::shutdown()
{
	if (_threadListener != nullptr){
		_threadListener->stop();
		delete _threadListener;
	}

	for (int i = 0; i < _workerNum; ++i)
	{
		_threadWorkerList[i]->stop();
		delete _threadWorkerList[i];
	}
	delete[] _threadWorkerList;

	if (_connPool != nullptr){
		_connPool->clear();
		delete _connPool;
	}
}


NetConnection* Network::createConn(SOCKET so, const char* ip, unsigned short port)
{
	NetConnection *conn = _connPool->createConn(so, ip, port);
	if (conn == nullptr){
		return nullptr;
	}

	if (!conn->init(this, so, ip, port)) {
		_connPool->removeConn(conn->getConnId());
		return nullptr;
	}

	net_msg_s msg;
	msg.conn_id = conn->getConnId();
	msg.type = NET_MSG_CONNECTED;
	msg.data = nullptr;
	msg.size = 0;
	pushMsg(msg);

	CreateIoCompletionPort((HANDLE)so, _iocp, (DWORD)so, 0);
	conn->recv();

	return conn;
}

void Network::removeConn(net_conn_id_t connId)
{
	NetConnection* conn = getConn(connId);
	conn->shutdown();
	if (_connPool->removeConn(connId))
	{
		net_msg_s msg;
		msg.conn_id = connId;
		msg.type = NET_MSG_DISCONNECTED;
		msg.data = nullptr;
		msg.size = 0;
		pushMsg(msg);
	}
}

void Network::_clearQueueMsgs() {
	while (!_queueMsgs.empty())
	{
		freeMsg(_queueMsgs.front());
		_queueMsgs.pop();
	}
}

void Network::freeMsg(net_msg_s& msg) {
	if (msg.type == NET_MSG_DATA && msg.data != nullptr) {
		delete[] msg.data;
	}
}

void Network::pushMsg(net_msg_s& msg){
	std::lock_guard<std::mutex> l(this->_msgQueueLock);
	_queueMsgs.push(msg);
}

bool Network::popMsg(net_msg_s& msg) {
	std::lock_guard<std::mutex> l(_msgQueueLock);
	if (_queueMsgs.size() == 0)
	{
		return false;
	}
	msg = _queueMsgs.front();
	_queueMsgs.pop();
	return true;
}