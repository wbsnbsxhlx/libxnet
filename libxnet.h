// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LIBXNET_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LIBXNET_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifndef libxnet_h__
#define libxnet_h__

#ifdef LIBXNET_EXPORTS
#define LIBXNET_API __declspec(dllexport)
#else
#define LIBXNET_API __declspec(dllimport)
#endif

#include"winerror.h"
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <stdint.h>

extern "C" {
	typedef int32_t network_t;
	typedef int32_t net_conn_id_t;

	const int INVALID_CONN_ID = -1;
	const int INVALID_NETWORK = -1;

	const int NET_MODE_DEFAULT = 0;
	const int NET_MODE_WEBSOCKET = 1;

	const int NET_MSG_CONNECTED = 1;
	const int NET_MSG_DISCONNECTED = 2;
	const int NET_MSG_DATA = 3;

	typedef struct net_msg_s {
		net_conn_id_t conn_id;
		int type;
		uint8_t* data;
		size_t size;
	} net_msg_s;

	LIBXNET_API network_t net_create(int worker_num, int max_client, int recv_buf_size, int send_buf_size, int mode);
	LIBXNET_API void net_destroy(network_t* network);

	LIBXNET_API int net_listen(network_t network, const char* local_addr, unsigned short port);
	LIBXNET_API net_conn_id_t net_connect(network_t network, const char* remote_addr, unsigned short port);
	LIBXNET_API void net_disconnect(network_t network, net_conn_id_t conn_id);

	LIBXNET_API int net_send(network_t network, net_conn_id_t connId, void* data, size_t size);
	LIBXNET_API int net_recv(network_t network, net_msg_s* msg);
	LIBXNET_API void net_free_msg(net_msg_s* msg);

	LIBXNET_API int net_get_conn_info(network_t id, net_conn_id_t connId, char* ip, uint16_t* port);
}

#endif // libxnet_h__