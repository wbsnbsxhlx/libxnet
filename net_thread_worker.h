#ifndef net_thread_worker_h__
#define net_thread_worker_h__

#include <thread>

struct net_fd_set;
class NetThreadWroker{
public:
	NetThreadWroker();

	bool start(int threadNum);
	void stop();
	void run();
private:
	net_fd_set* _readSet;
	net_fd_set* _writeSet;
	bool _isRun;
	std::thread* _thread;
	size_t _countMax;
};

#endif // net_thread_worker_h__
