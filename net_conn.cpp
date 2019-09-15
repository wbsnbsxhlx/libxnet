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
	:_ip(""),
	_port(0),
	_socket(INVALID_SOCKET)
{

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

