// libxnet.cpp : 定义 DLL 应用程序的导出函数。
//

#include "libxnet.h"
#include <windows.h>
#include "stdio.h"


// 这是导出变量的一个示例
LIBXNET_API int nlibxnet=0;

// 这是导出函数的一个示例。
LIBXNET_API int fnlibxnet(void)
{
	return 42;
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
