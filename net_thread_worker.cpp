#include "libxnet.h"
#include "net_thread_worker.h"
#include "net_log.h"
#include "net_conn.h"
#include <vector>

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
		if (_connList.size() <= 0){
			Sleep(1);
			continue;
		}

		NET_FD_ZERO(_readSet);
		NET_FD_ZERO(_writeSet);
		std::vector<NetConnection*> connVec;
		
		for (auto it = _connList.begin(); it != _connList.end(); ++it){
			NetConnection* conn = *it;

			if (conn->getSocket() == INVALID_SOCKET){
				_eraseConn(it);
				continue;
			}
			connVec.push_back(conn);

			NET_FD_SET(conn->getSocket(), _readSet);

			if (conn->hasSendData()) {
				NET_FD_SET(conn->getSocket(), _writeSet);
			}
		}

		timeval tv = { 0, 10 * 1000 };//‘›∂®10√Î
		int num = select(0, (fd_set*)_readSet, (fd_set*)_writeSet, NULL, &tv);

		for (int i = 0; (i < connVec.size()) && (num > 0); ++i) {
			NetConnection* conn = connVec[i];

			if (nullptr == conn)
				continue;

			bool proc = false;

			if (conn->getSocket() != INVALID_SOCKET) {
				if (NET_FD_ISSET(conn->getSocket(), _readSet)) {
					conn->recv();
					proc = true;
				}
			}

			if (conn->getSocket() != INVALID_SOCKET) {
				if (NET_FD_ISSET(conn->getSocket(), _writeSet)) {
					conn->send();
					proc = true;
				}
			}

			if (proc)
				num--;
		}
	}
}

bool NetThreadWroker::addConn(NetConnection* conn)
{
	if (_connList.size() >= _countMax){
		log(LOG_ERROR, "workerList is full");
		return false;
	}

	conn->retain();
	return true;
}

bool NetThreadWroker::removeConn(NetConnection* conn)
{
	_eraseConn(conn);
}

bool NetThreadWroker::_eraseConn(std::list<NetConnection*>::iterator it)
{
	NetConnection* conn = *it;
	conn->release();
	_connList.erase(it);
}

bool NetThreadWroker::_eraseConn(NetConnection* conn)
{
	conn->release();
	_connList.remove(conn);
}

