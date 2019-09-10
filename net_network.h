#ifndef net_network_h__
#define net_network_h__

#include <windows.h>
#include <thread>
#include "net_thread_listener.h"

class Network
{
public:
	Network();
	~Network();

	static Network* create(int threadNum, int maxClient, int recvBufSize, int sendBufSize);

	bool init(int threadNum, int maxClient, int recvBufSize, int sendBufSize);
	bool listen(const char* local_addr, unsigned short port);
	void connect(const char* remote_addr, unsigned short port);

	void shutdown();

private:
	int _threamNum;
	int _maxClient;
	int _recvBufSize;
	int _sendBufSize;

	NetThreadListener* _threadListener;
};

#endif // net_network_h__