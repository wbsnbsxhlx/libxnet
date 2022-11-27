#ifndef net_thread_listener_h__
#define net_thread_listener_h__

#include <thread>
#include "libxnet.h"

class Network;
class NetThreadListener{
public:
	NetThreadListener();
	bool start(const char* local_addr, unsigned short port);
	void stop();
	void run();
	
	bool attachNetwork(Network* network);
private:
	bool _isRun;
	std::thread* _thread;
	SOCKET _socket;
	Network* _network;
};

#endif // net_thread_listener_h__
