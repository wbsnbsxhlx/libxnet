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

#include <stdint.h>
#include <windows.h>


typedef int32_t network_t;
typedef int32_t net_conn_id_t;
extern "C" {
	LIBXNET_API network_t net_create(int worker_num, int max_client, int recv_buf_size, int send_buf_size);
	LIBXNET_API void net_destroy(network_t* network);

	LIBXNET_API int net_listen(network_t network, const char* local_addr, unsigned short port);
	LIBXNET_API net_conn_id_t net_connect(network_t network, const char* remote_addr, unsigned short port);
}
// �����Ǵ� libxnet.dll ������
class LIBXNET_API SocketObject {
public:
	SocketObject(void);

	static SocketObject* CreateObject();
	void Listen(const char* ip, uint16_t port);
	SOCKET Accept();
	bool Valid();

	void Send(const char* data, int len);
	void Recv(char* buf, int len);

private:
	void bind(const char* ip, uint16_t port);
	void createSocket();

private:
	SOCKET _socket;

};

class LIBXNET_API Connection {
public:
	Connection(void);

	const char* GetIp();
	const uint16_t GetPort();

	void Send(const char* data, int len);
	void Recv(char* buf, int len);
private:
	SOCKET _socket;
	char _ip[16];
	uint16_t _port;
};

extern LIBXNET_API int nlibxnet;

LIBXNET_API int fnlibxnet(void);


#endif // libxnet_h__