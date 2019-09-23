#include "net_conn.h"
#include "net_log.h"
#include "net_network.h"
#include "net_header.h"
#include "libxnet.h"
#include <winsock2.h>
#include <windows.h>

NetConnection* NetConnection::create(SOCKET so, const char* ip, unsigned short port)
{
	NetConnection* ret = new NetConnection();
	if (!ret->init(so, ip, port)) {
		delete ret;
		ret = nullptr;
	}
	return ret;
}

NetConnection::NetConnection()
	:_connId(INVALID_CONN_ID),
	_network(nullptr),
	_ip(""),
	_port(0),
	_socket(INVALID_SOCKET),
	_refCount(0)
{

}

NetConnection::~NetConnection()
{
	if (_socket != INVALID_SOCKET){
		closesocket(_socket);
	}
}

void NetConnection::retain()
{
	++_refCount;
}

void NetConnection::release()
{
	if (--_refCount <= 0){
		delete this;
	}
}

bool NetConnection::init(SOCKET so, const char* ip, unsigned short port)
{
	if (_socket != INVALID_SOCKET)
	{
		log(LOG_ERROR, "_socket is exsist!");
		return false;
	}
	if (so == INVALID_SOCKET)
	{
		log(LOG_ERROR, "param so is invalid");
		return false;
	}
	strcpy_s(_ip, ip);
	_port = port;

	return true;
}

bool NetConnection::setNetwork(Network* network)
{
	if (_network != nullptr){
		log(LOG_ERROR, "network is exsist!");
		return false;
	}

	_network = network;

	return true;
}

bool NetConnection::write(void* data, size_t size)
{
	_sendBuffer.write(data, size);
}

bool NetConnection::initBufSize(int sendBufSize, int recvBufSize)
{
	_sendBuffer.init(sendBufSize);
	_recvBuffer.init(recvBufSize);
	return true;
}

bool NetConnection::setConnId(net_conn_id_t connId)
{
	if (_connId != INVALID_CONN_ID){
		log(LOG_ERROR, "connection is exsist! connId=", _connId);
		return false;
	}
	_connId = connId;
	return true;
}

bool NetConnection::send()
{
	int curLen = 0;
	while (curLen != size)
	{
		int sendLen = ::send(_socket, (const char*)data, size + curLen, 0);
		if (sendLen == 0){
			log(LOG_ERROR, "send error curLen:%d, size:%d", curLen, size);
			return false;
		}
		curLen += sendLen;
	}

	return true;
}

void NetConnection::recv()
{
	while (_socket != INVALID_SOCKET){
		size_t size = 0;
		uint8_t* buf = _recvBuffer.pickWrite(size);

		if (size == 0) {
			log(LOG_ERROR, "recv buffer is full.");
			close();
			break;
		}

		int len = ::recv(_socket, (char*)buf, size, 0);
		if (len > 0) {
			_recvBuffer.writeLen(len);
			if (len < size)
				break;
		}else {
			DWORD err = WSAGetLastError();

			if (err == WSAEWOULDBLOCK){
				break;
			}else{
				close();
				break;
			}
		}
	}

	pushMsg();
}

void NetConnection::pushMsg()
{
	if (_recvBuffer.empty()){
		return;
	}

	NetMessage msg;
	while (_recvBuffer.makeMsg(msg))
	{
		msg.conn_id = _connId;
		msg.type = NET_MSG_DATA;
		_network->pushMsg(msg);
		_recvBuffer.readLen(sizeof(NetMsgHeader) + msg.size);
	}

// 	size_t bufSize = _recvBuffer.length();
// 	uint8_t* buf = new uint8_t[bufSize];
// 	_recvBuffer.copyTo(buf);
// 
// 	NetMsgHeader header;
// 	memcpy(&header, buf, sizeof(header));
// 	_recvBuffer.readLen(header.size);
// 	
// 	NetMessage msg;
// 	msg.conn_id = _connId;
// 	msg.type = NET_MSG_DATA;
// 	msg.data = buf;
// 	msg.size = header.size;
// 	_network->pushMsg(msg);
}

void NetConnection::close()
{
	shutdown();
	_network->removeConn(this);
}

void NetConnection::shutdown()
{
	if (_socket != INVALID_SOCKET){
		::shutdown(_socket, SD_BOTH);
		::closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}

