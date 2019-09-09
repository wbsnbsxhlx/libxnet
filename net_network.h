#ifndef net_network_h__
#define net_network_h__

#include <windows.h>

class Network
{
public:
	Network();
	~Network();

	bool init(int threadNum, int maxClient, int recvBufSize, int sendBufSize);
	BOOL listen(const char* local_addr, unsigned short port);
	void connect(const char* remote_addr, unsigned short port);

	void shutdown();

private:
	SOCKET _socket;
	int _threamNum;
};

#endif // net_network_h__