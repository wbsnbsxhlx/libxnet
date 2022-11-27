// libxnet.cpp : 定义 DLL 应用程序的导出函数。
//

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
		return network->listen(ip, port) ? 0 : -1;
	}else{
		log(LOG_ERROR, "id:%d", id);
	}
	return -1;
}

LIBXNET_API int net_send(network_t id, net_conn_id_t connId, void* data, size_t size)
{
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network == nullptr)
	{
		log(LOG_ERROR, "id:%d", id);
		return -1;
	}

	NetConnection* conn = network->getConn(connId);
	if (conn == nullptr)
	{
		log(LOG_ERROR, "conn is not exsist id:%d", connId);
		return 0;
	}
	conn->write(data, size);
	return 0;
}
