#include "libxnet.h"
#include "net_thread_worker.h"
#include "net_log.h"
#include "net_conn.h"
#include <vector>

NetThreadWroker::NetThreadWroker()
	:_isRun(false),
	_thread(nullptr) {
}

bool NetThreadWroker::start(HANDLE iocp) {
	if (_isRun) {
		log(LOG_ERROR, "listener is running!");
		return false;
	}
	if (_thread != nullptr) {
		log(LOG_ERROR, "thread is exsist!");
		return false;
	}

	_iocp = iocp;
	_isRun = true;
	_thread = new std::thread(&NetThreadWroker::run, this);
	return true;
}

void NetThreadWroker::stop() {
	_isRun = false;
	_thread->join();

	if (nullptr != _thread) {
		delete _thread;
		_thread = nullptr;
	}
}

void NetThreadWroker::run() {
	BOOL    bResult;
	DWORD   dwNumRead;
	DWORD	dwKey;
	LPOVERLAPPED lpOverlapped;
	while (_isRun) {
		bResult = GetQueuedCompletionStatus(_iocp, &dwNumRead, &dwKey, &lpOverlapped, 0);
		if (lpOverlapped && bResult) {
			NetConnectionOverlapped* no = (NetConnectionOverlapped*)lpOverlapped;
			if (no->isSender) {
				no->conn->send();
			} else {
				no->conn->pushMsg();
				no->conn->recv();
			}
		}
	}
}
