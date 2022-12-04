#include "net_conn.h"
#include "net_log.h"
#include "net_network.h"
#include "libxnet.h"

NetConnection::NetConnection()
	:_connId(INVALID_CONN_ID),
	_network(nullptr),
	_port(0),
	_socket(INVALID_SOCKET){
	sender = new NetConnectionOverlapped();
	recver = new NetConnectionOverlapped();
	sender->isSender = true;
	recver->isSender = false;
	sender->conn = recver->conn = this;
}

NetConnection::~NetConnection() {
	delete sender;
	delete recver;
}

bool NetConnection::init(Network* network, SOCKET so, const char* ip, unsigned short port) {
	if (_socket != INVALID_SOCKET) {
		log(LOG_ERROR, "_socket is exsist!");
		return false;
	}
	if (so == INVALID_SOCKET) {
		log(LOG_ERROR, "param so is invalid");
		return false;
	}

	if (_network != nullptr) {
		log(LOG_ERROR, "network is exsist!");
		return false;
	}

	_network = network;
	_socket = so;
	strcpy_s(_ip, ip);
	_port = port;

	_recvBuffer.clear();
	_sendBuffer.clear();

	return true;
}

bool NetConnection::initBufSize(int sendBufSize, int recvBufSize) {
	_sendBuffer.init(sendBufSize);
	_recvBuffer.init(recvBufSize);
	return true;
}

bool NetConnection::write(void* data, size_t size) {
	std::lock_guard<std::mutex> l(_sendBufLock);
	onWrite(data, size);
	return false;
}

bool NetConnection::setConnId(net_conn_id_t connId) {
	if (_connId != INVALID_CONN_ID) {
		log(LOG_ERROR, "connection is exsist! connId=", _connId);
		return false;
	}
	_connId = connId;
	return true;
}

bool NetConnection::send() {
	std::lock_guard<std::mutex> l(_sendBufLock);
	if (_sendBuffer.length() == 0){
		return false;
	}

	DWORD dw = 0;
	WSABUF wsabuf;
	size_t len;
	wsabuf.buf = (char *)_sendBuffer.pickRead(&len);
	wsabuf.len = len;

	if (0 != WSASend(_socket, &wsabuf, 1, &dw, 0, sender, NULL)) {
		int error = GetLastError();
		if (error != ERROR_IO_PENDING) {
			log(LOG_ERROR, "connection is exsist! %s", error);
			close();
			return false;
		}
	}

	return true;
}

void NetConnection::recvedLength(size_t len) {
	std::lock_guard<std::mutex> l(_recvBufLock);
	_recvBuffer.writeLen(len);
}

void NetConnection::sendedLength(size_t len) {
	std::lock_guard<std::mutex> l(_sendBufLock);
	_sendBuffer.readLen(len);
}

void NetConnection::recv() {
	DWORD dw = 0; DWORD fg = 0;
	WSABUF wsabuf;
	size_t len;

	std::lock_guard<std::mutex> l(_recvBufLock);
	wsabuf.buf = (char *)_recvBuffer.pickWrite(&len);
	wsabuf.len = len;
	if (WSARecv(_socket, &wsabuf, 1, &dw, &fg, recver, NULL) != 0) {
		int error = GetLastError();
		if (error != ERROR_IO_PENDING) {
			log(LOG_ERROR, "%d", WSAGetLastError());
			close();
		}
	}
}

void NetConnection::procRecv() {
	std::lock_guard<std::mutex> l(_recvBufLock);
	if (_recvBuffer.empty()) {
		return;
	}
	while(onProcRecv());
}

void NetConnection::close() {
	if (_network != nullptr) {
		_network->removeConn(getConnId());
	}
}

void NetConnection::shutdown() {
	std::lock_guard<std::mutex> l(_shutdownLock);
	if (_socket != INVALID_SOCKET) {
		::shutdown(_socket, SD_BOTH);
		::closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
	_network = nullptr;
}