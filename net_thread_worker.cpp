#include "libxnet.h"
#include "net_thread_worker.h"
#include "net_log.h"

typedef struct net_fd_set {
	u_int fd_count;
	SOCKET fd_array[1];
} net_fd_set;
#define NET_FD_SET(fd, set) do {\
		u_int __i;\
		for (__i = 0; __i < ((net_fd_set*)(set))->fd_count; ++__i) {\
			if ((fd) == ((net_fd_set*)(set))->fd_array[__i])\
				break;\
						}\
		if (__i == ((net_fd_set*)(set))->fd_count) {\
			((net_fd_set*)(set))->fd_array[__i] = (fd);\
			++((net_fd_set*)(set))->fd_count;\
						}\
	} while (0)
#define NET_FD_ZERO    FD_ZERO
#define NET_FD_CLR     FD_CLR
#define NET_FD_ISSET   FD_ISSET
#define NET_FD_COPY(dest, src) do {\
		(dest)->fd_count = (src)->fd_count;\
		memcpy((dest)->fd_array, (src)->fd_array, sizeof(int) * (src)->fd_count);\
	} while (0)

NetThreadWroker::NetThreadWroker()
	:_isRun(false),
	_thread(nullptr),
	_readSet(nullptr),
	_writeSet(nullptr)
{
}

bool NetThreadWroker::start(int threadNum)
{
	if (_isRun){
		log(LOG_ERROR, "listener is running!");
		return false;
	}
	if (_thread != nullptr || nullptr != _readSet || nullptr != _writeSet){
		log(LOG_ERROR, "thread is exsist!");
	}

	_countMax = threadNum;

	size_t set_size = sizeof(net_fd_set) + (_countMax - 1) * sizeof(int);
	_readSet = (net_fd_set*)malloc(set_size);
	_writeSet = (net_fd_set*)malloc(set_size);

	_isRun = true;
	_thread = new std::thread(&NetThreadWroker::run, this);
}

void NetThreadWroker::stop()
{
	_isRun = false;
	_thread->join();

	if (nullptr != _thread){
		delete _thread;
		_thread = nullptr;
	}

	if (nullptr != _readSet) {
		free(_readSet);
		_readSet = nullptr;
	}

	if (nullptr != _writeSet) {
		free(_writeSet);
		_writeSet = nullptr;
	}
}

void NetThreadWroker::run()
{
	while (_isRun){
		Sleep(1);
	}
}

