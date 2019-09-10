#include "net_network.h"
#include "net_log.h"
#include <thread>

Network::Network()
	:_threamNum(0),
	_maxClient(0),
	_recvBufSize(0),
	_sendBufSize(0),
	_threadList(nullptr)
{
}

Network::~Network()
{
}

Network* Network::create(int threadNum, int maxClient, int recvBufSize, int sendBufSize)
{
	Network *ret = new Network();
	if (!ret->init(threadNum, maxClient, recvBufSize, sendBufSize))
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool Network::init(int threadNum, int maxClient, int recvBufSize, int sendBufSize)
{
	if (threadNum < 1)
	{
		log(LOG_ERROR, "[threadNum=%d] threadNum must >1!", threadNum);
	}
	if (maxClient < 1)
	{
		log(LOG_ERROR, "[maxClient=%d] maxClient must >1!", maxClient);
	}
	if (recvBufSize < 64)
	{
		log(LOG_ERROR, "[recvBufSize=%d] recvBufSize must >64!", recvBufSize);
	}
	if (sendBufSize < 64)
	{
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

}

