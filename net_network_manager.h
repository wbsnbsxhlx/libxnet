#include <atomic>
#include <AccCtrl.h>
#include "libxnet.h"
#include <map>
#include <queue>

class NetworkManager
{
/********************singleton begin**********************/
public:
	static NetworkManager *getInstance();
private:
	static std::atomic<NetworkManager*> instance;
	NetworkManager();
	~NetworkManager();
/********************singleton end**********************/

public:
	network_t createNetwork(int thread_num, int max_client, int recv_buf_size, int send_buf_size);
	bool destroyNetwork(network_t);

private:
	std::map<network_t, Network*> _networkMap;
	std::queue<network_t> _networkFreeQueue;
	network_t _networkIdMax;
};