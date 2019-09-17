// libxnet.cpp : 定义 DLL 应用程序的导出函数。
//

#include <windows.h>
#include "stdio.h"

#include "libxnet.h"
#include "net_network_manager.h"
#include "net_network.h"
#include "net_log.h"


LIBXNET_API network_t net_create(int worker_num, int max_client, int recv_buf_size, int send_buf_size)
{
	return NetworkManager::getInstance()->createNetwork(worker_num, max_client, recv_buf_size, send_buf_size);
}

LIBXNET_API void net_destroy(network_t* network)
{
	NetworkManager::getInstance()->destroyNetwork(*network);
	*network = -1;
}

LIBXNET_API int net_listen(network_t id, const char* ip, unsigned short port)
{
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network != nullptr)
	{
		network->listen(ip, port);
	}
	else{
		log(LOG_ERROR, "id:%d", id);
	}
}

LIBXNET_API int net_send(network_t id, net_conn_id_t connId, void* data, size_t size)
{
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network == nullptr)
	{
		log(LOG_ERROR, "id:%d", id);
		return 0;
	}

	NetConnection* conn = network->getConnPool()->getConn(connId);
	if (conn == nullptr)
	{
		log(LOG_ERROR, "conn is not exsist id:%d", connId);
		return 0;
	}
	conn->send(data, size);
	return 1;
}

SocketObject::SocketObject(void)
	:_socket(INVALID_SOCKET)
{
}

SocketObject* SocketObject::CreateObject()
{

	SocketObject* obj = new SocketObject();
	obj->createSocket();

	return obj;
}

void SocketObject::Listen(const char* ip, uint16_t port)
{
	this->bind(ip, port);
	int ret = listen(_socket, 10);
	if (ret == SOCKET_ERROR){
		printf("[libxnet] SocketObject listen error");
		return;
	}
}

SOCKET SocketObject::Accept()
{
	sockaddr_in addr;
	int len = sizeof(addr);
	SOCKET client = accept(_socket, (sockaddr*)&addr, &len);
	
	return client;
}

bool SocketObject::Valid()
{
	return _socket != INVALID_SOCKET;
}

void SocketObject::Send(const char* data, int len)
{
	int curLen = 0;
	while(curLen != len)
	{
		int sendLen = ::send(_socket, data, len + curLen, 0);
		if (sendLen == 0){
			printf("[libxnet] SocketObject send error curLen:%d, len:%d", curLen, len);
			break;
		}
		curLen += sendLen;
	}
}

void SocketObject::Recv(char* buf, int len)
{
	int retLen = recv(_socket, buf, len, 0);
	if (retLen == SOCKET_ERROR){
		printf("[libxnet] SocketObject recv error retLen:%d, len:%d", retLen, len);
	}
}

void SocketObject::bind(const char* ip, uint16_t port)
{
	if (_socket == -1){
		return;
	}
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	int ret = ::bind(_socket, (sockaddr*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR){
		printf("[libxnet] SocketObject bind %s:%d error", ip, port);
		return;
	}
}

void SocketObject::createSocket()
{
	//第一个参数ipv4， 第二个tcp， 第三个协议类型
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == -1){
		printf("[libxnet] SocketObject socket create error");
		return;
	}
}

const char* Connection::GetIp()
{
	return _ip;
}

const uint16_t Connection::GetPort()
{
	return _port;
}
