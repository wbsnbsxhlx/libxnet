#include "net_network.h"

Network::Network()
	:_socket(INVALID_SOCKET),
	_threamNum(0)
{
}

Network::~Network()
{
}

bool Network::init(int threadNum, int maxClient, int recvBufSize, int sendBufSize)
{
	_threamNum = threadNum;
	if (_socket != INVALID_SOCKET)
	{

		return false;
	}
}

