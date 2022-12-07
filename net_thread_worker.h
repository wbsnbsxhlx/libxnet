#ifndef net_thread_worker_h__
#define net_thread_worker_h__

#include <thread>

//struct net_fd_set;
class NetThreadWroker{
public:
	NetThreadWroker();

	bool start(HANDLE iocp);
	void stop();
	void run();
	//bool addConn(NetConnection* conn);
	//bool removeConn(NetConnection* conn);
private:
	//bool _eraseConn(std::list<NetConnection*>::iterator it);
	//bool _eraseConn(NetConnection* conn);
	//net_fd_set* _readSet;
	//net_fd_set* _writeSet;
	//size_t _countMax;
	//std::list<NetConnection*> _connList;
	HANDLE _iocp;

	bool _isRun;
	std::thread* _thread;
};

#endif // net_thread_worker_h__
