#include <windows.h>
#include "net_thread_listener.h"
#include "net_conn.h"
#include "net_log.h"
#include "net_network.h"

NetThreadListener::NetThreadListener()
	:_isRun(false),
	_thread(nullptr),
	_socket(INVALID_SOCKET),
	_parent(nullptr)
{

}

bool NetThreadListener::start(const char* local_addr, unsigned short port)
{
	if (_isRun){
		log(LOG_ERROR, "listener is running!");
		return false;
	}
	if (_socket !=INVALID_SOCKET){
		log(LOG_ERROR, "socket is exsist!");
		return false;
	}
	if (_thread != nullptr){
		log(LOG_ERROR, "thread is exsist!");
	}

	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
		log(LOG_ERROR, "listen error, ip: %s:%d", local_addr, port);
		return false;
	}

	_isRun = true;
	_thread = new std::thread(&NetThreadListener::run, this);
	return true;
}

void NetThreadListener::stop()
{
	_isRun = false;
	_thread->join();
	delete _thread;
	_thread = nullptr;
}

void NetThreadListener::run()
{
	while (_isRun){
		Sleep(1);

		sockaddr_in addr;
		int len = sizeof(addr);
		SOCKET client = accept(_socket, (sockaddr*)&addr, &len);
		//todo add connection
		net_conn_id_t id = _parent->getConnPool()->newConn(client, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	}
}

bool NetThreadListener::setParent(Network* network)
{
	if (_parent != nullptr){
		log(LOG_ERROR, "parent is exsist!");
		return false;
	}

	_parent = network;

	return true;
}

