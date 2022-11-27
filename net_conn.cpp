#include "net_conn.h"
#include "net_log.h"
#include "net_network.h"
#include "net_header.h"
#include "libxnet.h"

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
	_port(0),
	_socket(INVALID_SOCKET),
	_refCount(0)
{
	sender.isSender = true;
	recver.isSender = false;
	sender.conn = recver.conn = this;
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
	_socket = so;
	strcpy_s(_ip, ip);
	_port = port;

	recv();

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
	return _sendBuffer.write(data, size);
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
	size_t size;
	uint8_t *buf = _sendBuffer.pickWrite(size);
	while (curLen != size)
	{
		int sendLen = ::send(_socket, (const char*)buf, size + curLen, 0);
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
	DWORD dw = 0; DWORD fg = 0;
	WSABUF wsabuf;
	size_t len;
	wsabuf.buf = (char *)_recvBuffer.pickWrite(len);
	wsabuf.len = len;
	WSARecv(_socket, &wsabuf, 1, &dw, &fg, &recver, NULL);
	//while (_socket != INVALID_SOCKET){
	//	size_t size = 0;
	//	uint8_t* buf = _recvBuffer.pickWrite(size);
	//
	//	if (size == 0) {
	//		log(LOG_ERROR, "recv buffer is full.");
	//		close();
	//		break;
	//	}
	//
	//	int len = ::recv(_socket, (char*)buf, size, 0);
	//	if (len > 0) {
	//		_recvBuffer.writeLen(len);
	//		if (len < size)
	//			break;
	//	}else {
	//		DWORD err = WSAGetLastError();
	//
	//		if (err == WSAEWOULDBLOCK){
	//			break;
	//		}else{
	//			close();
	//			break;
	//		}
	//	}
	//}
	//
	//pushMsg();
}

void NetConnection::pushMsg()
{
	if (_recvBuffer.empty()){
		return;
	}

	net_msg_s msg;
	while (_recvBuffer.makeMsg(msg))
	{
		msg.conn_id = _connId;
		msg.type = NET_MSG_DATA;
		_network->pushMsg(msg);
		_recvBuffer.readLen(sizeof(NetMsgHeader) + msg.size);
	}
}

void NetConnection::close()
{
	_network->removeConn(this);
	shutdown();

	net_msg_s msg;
	msg.conn_id = _connId;
	msg.type = NET_MSG_DISCONNECTED;
	msg.data = nullptr;
	msg.size = 0;
	_network->pushMsg(msg);
}

void NetConnection::shutdown()
{
	if (_socket != INVALID_SOCKET){
		::shutdown(_socket, SD_BOTH);
		::closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}