#ifndef net_network_manager_h__
#define net_network_manager_h__

#include "libxnet.h"
#include <atomic>
#include <AccCtrl.h>
#include <map>
#include <queue>
#include <mutex>

class Network;
class NetworkManager
{
/********************singleton begin**********************/
public:
	static NetworkManager* getInstance();
private:
	static std::atomic<NetworkManager*> instance;
	NetworkManager();
	~NetworkManager();
/********************singleton end**********************/

public:
	network_t createNetwork(int thread_num, int max_client, int recv_buf_size, int send_buf_size, int mode);
	bool destroyNetwork(network_t id);
	Network* getNetwork(network_t id);

private:
	network_t _getFreeNetId();
	void _freeNetId(network_t id);

	std::map<network_t, Network*> _networkMap;
	std::vector<network_t> _freeNetworkVec;
	network_t _networkIdMax;

	std::mutex _nwmapLock;
};

#endif // net_network_manager_h__