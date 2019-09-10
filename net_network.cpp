#include "net_network.h"
#include "net_log.h"
#include <thread>

Network::Network()
	:_threamNum(0),
	_maxClient(0),
	_recvBufSize(0),
	_sendBufSize(0),
	_threadListener(nullptr)
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

	_threamNum = threadNum;
	_maxClient = maxClient;
	_recvBufSize = recvBufSize;
	_sendBufSize = sendBufSize;

	return true;
}

bool Network::listen(const char* local_addr, unsigned short port)
{
	if (_threadListener != nullptr) {
		log(LOG_ERROR, "_threadListener is exsist");
		return false;
	}

	SOCKET so = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (so == INVALID_SOCKET) {
		log(LOG_ERROR, "listen error: ip:%s,port:%d", local_addr, port);
		return false;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(local_addr);

	int ret = bind(so, (sockaddr *)&addr, port);
	if (ret == SOCKET_ERROR) {
		log(LOG_ERROR, "bind %s:%d error", local_addr, port);
		return false;
	}

	ret = ::listen(so, 10);
	if (ret == SOCKET_ERROR) {
		log(LOG_ERROR, "listen error, ip: %s:%d", local_addr, port);
		return false;
	}

	_threadListener = new NetThreadListener();

	return true;
}

void Network::shutdown()
{
	if (_threadListener != nullptr){
		delete _threadListener;
	}
}

