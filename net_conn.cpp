#include "net_conn.h"
#include "net_log.h"

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

bool NetConnection::send()
{
	int curLen = 0;
	while (curLen != size)
	{
		int sendLen = ::send(_socket, (const char*)data, size + curLen, 0);
		if (sendLen == 0){
			log(LOG_ERROR,"send error curLen:%d, size:%d", curLen, size);
			return false;
		}
		curLen += sendLen;
	}

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

bool NetConnection::setConnId(net_conn_id_t connId)
{
	if (_connId != INVALID_CONN_ID){
		log(LOG_ERROR, "connection is exsist! connId=", _connId);
		return false;
	}
	_connId = connId;
	return true;
}

void NetConnection::recv()
{
	while (true){
		size_t size = 0;
		uint8_t* buf = _recvBuffer.pick(size);

		if (size == 0) {
			log(LOG_ERROR, "recv buffer is full.");
			destroy();
			return;
		}

		int len = ::recv(_socket, (char*)buf, size, 0);
		if (len > 0) {
			_recvBuffer.add(len);
			if (len < size)
				break;
		}else {
			if (!sendError)
				return;

			break;
		}
	}
}

