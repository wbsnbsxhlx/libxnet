#include "net_thread_listener.h"
#include "net_conn.h"
#include "net_log.h"
#include "net_network.h"

NetThreadListener::NetThreadListener()
	:_isRun(false),
	_thread(nullptr),
	_socket(INVALID_SOCKET),
	_network(nullptr) {

}

bool NetThreadListener::start(const char* local_addr, unsigned short port) {
	if (_isRun) {
		log(LOG_ERROR, "listener is running!");
		return false;
	}
	if (_socket != INVALID_SOCKET) {
		log(LOG_ERROR, "socket is exsist!");
		return false;
	}
	if (_thread != nullptr) {
		log(LOG_ERROR, "thread is exsist!");
	}

	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_socket == INVALID_SOCKET) {
		log(LOG_ERROR, "socket create error: ip:%s,port:%d", local_addr, port);
		return false;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(local_addr);

	int ret = bind(_socket, (sockaddr*)& addr, port);
	if (ret == SOCKET_ERROR) {
		log(LOG_ERROR, "bind %s:%d error", local_addr, port);
		return false;
	}

	ret = ::listen(_socket, 10);
	if (ret == SOCKET_ERROR) {
		closesocket(_socket);
		_socket = INVALID_SOCKET;
		log(LOG_ERROR, "listen error, ip: %s:%d", local_addr, port);
		return false;
	}

	_isRun = true;
	_thread = new std::thread(&NetThreadListener::run, this);
	return true;
}

void NetThreadListener::stop() {
	_isRun = false;
	_thread->join();
	delete _thread;
	_thread = nullptr;
}

void NetThreadListener::run() {
	while (_isRun) {
		sockaddr_in addr;
		int len = sizeof(addr);
		SOCKET client = accept(_socket, (sockaddr*)&addr, &len);
		//todo add connection
		if (_network->createConn(client, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)) == nullptr){
			shutdown(client, SD_BOTH);
			closesocket(client);
		}
	}
}

bool NetThreadListener::attachNetwork(Network* network) {
	if (_network != nullptr) {
		log(LOG_ERROR, "parent is exsist!");
		return false;
	}

	_network = network;

	return true;
}