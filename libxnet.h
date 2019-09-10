// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LIBXNET_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LIBXNET_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
// 此类是从 libxnet.dll 导出的
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