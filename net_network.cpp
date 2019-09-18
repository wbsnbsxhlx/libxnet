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
}

Network* Network::create(int threadNum, int maxClient, int recvBufSize, int sendBufSize)
{
	Network *ret = new Network();
	if (!ret->init(threadNum, maxClient, recvBufSize, sendBufSize)){
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool Network::init(int threadNum, int maxClient, int recvBufSize, int sendBufSize)
{
	if (threadNum < 1){
		log(LOG_ERROR, "[threadNum=%d] threadNum must >1!", threadNum);
	}
	if (maxClient < 1){
		log(LOG_ERROR, "[maxClient=%d] maxClient must >1!", maxClient);
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
	_connPool->init(sendBufSize, recvBufSize);

	_threadWorkerList = new NetThreadWroker*[threadNum];
	for (int i = 0; i < threadNum; ++i){
		_threadWorkerList[i] = new NetThreadWroker();
		_threadWorkerList[i]->start(maxClient / threadNum);
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
	_threadListener->setParent(this);
	return _threadListener->start(local_addr, port);
}

void Network::shutdown()
{
	if (_threadListener != nullptr){
		delete _threadListener;
	}
	if (_connPool != nullptr){
		delete _connPool;
	}
}

