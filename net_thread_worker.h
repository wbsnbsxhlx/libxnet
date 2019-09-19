#ifndef net_thread_worker_h__
#define net_thread_worker_h__

#include <thread>
#include <list>

struct net_fd_set;
class NetThreadWroker{
public:
	NetThreadWroker();

	bool start(int threadNum);
	void stop();
	void run();
	bool addConn(NetConnection* conn);
private:
	bool _eraseConn(std::list<NetConnection*>::iterator it);
	net_fd_set* _readSet;
	net_fd_set* _writeSet;
	bool _isRun;
	std::thread* _thread;
	size_t _countMax;
	std::list<NetConnection*> _connList;
};

#endif // net_thread_worker_h__
