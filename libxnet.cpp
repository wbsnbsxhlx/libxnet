// libxnet.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdio.h"

#include "libxnet.h"
#include "net_network_manager.h"
#include "net_network.h"
#include "net_log.h"
#include "net_conn.h"

LIBXNET_API network_t net_create(int worker_num, int max_client, int recv_buf_size, int send_buf_size, int mode) {
	return NetworkManager::getInstance()->createNetwork(worker_num, max_client, recv_buf_size, send_buf_size, mode);
}

LIBXNET_API void net_destroy(network_t* network) {
	NetworkManager::getInstance()->destroyNetwork(*network);
	*network = -1;
}

LIBXNET_API int net_listen(network_t id, const char* ip, unsigned short port) {
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network == nullptr) {
		net_log_error("network is null:%d", id);
		return -1;
	}
	return network->listen(ip, port) ? 0 : -1;
}

LIBXNET_API void net_disconnect(network_t id, net_conn_id_t conn_id) {
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network == nullptr) {
		net_log_error("network is null:%d", id);
		return;
	}
	NetConnection* conn = network->getConn(conn_id);
	if (conn == nullptr) {
		log(LOG_ERROR, "conn is not exsist id:%d", conn_id);
		return;
	}
	conn->setCloseFlag(true);
}


LIBXNET_API int net_send(network_t id, net_conn_id_t connId, void* data, size_t size) {
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network == nullptr) {
		log(LOG_ERROR, "id:%d", id);
		return -1;
	}

	NetConnection* conn = network->getConn(connId);
	if (conn == nullptr) {
		log(LOG_ERROR, "conn is not exsist id:%d", connId);
		return 0;
	}
	if (conn->write(data, size)) {
		conn->send();
	}
	return 0;
}

LIBXNET_API int net_recv(network_t id, net_msg_s* msg) {
	Network* network = NetworkManager::getInstance()->getNetwork(id);
	if (network != nullptr) {
		return network->popMsg(*msg) ? 0 : -1;
	}
	return -1;
}

LIBXNET_API void net_free_msg(net_msg_s* msg) {
	if (msg->type == NET_MSG_DATA && msg->data != nullptr) {
		delete[] msg->data;
	}
}