#include "net_network_manager.h"
#include "net_network.h"
#include "net_log.h"
#include "libxnet.h"
#include <atomic>
#include <mutex>

std::atomic<NetworkManager*> NetworkManager::instance = nullptr;

NetworkManager::NetworkManager()
	:_networkIdMax(1) {

	WSADATA ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
}

NetworkManager::~NetworkManager() {
	WSACleanup();
}

network_t NetworkManager::createNetwork(int thread_num, int max_client, int recv_buf_size, int send_buf_size, int mode) {
	std::lock_guard<std::mutex> l(_nwmapLock);

	Network *network = new Network(mode);
	if (!network->init(thread_num, max_client, recv_buf_size, send_buf_size)) {
		delete network;
		return INVALID_NETWORK;
	}

	network_t id = _getFreeNetId();
	_networkMap[id] = network;

	return id;
}

bool NetworkManager::destroyNetwork(network_t id) {
	std::lock_guard<std::mutex> l(_nwmapLock);

	if (_networkMap.count(id) == 0) {
		net_log_error("networkid %d is not exsist", id);
		return false;
	}

	Network* network = _networkMap[id];
	network->shutdown();
	delete network;

	_networkMap.erase(id);
	_freeNetId(id);

	return true;
}

Network* NetworkManager::getNetwork(network_t id) {
	std::lock_guard<std::mutex> l(_nwmapLock);
	Network* ret = nullptr;

	if (_networkMap.count(id) > 0) {
		ret = _networkMap[id];
	}
	return ret;
}

network_t NetworkManager::_getFreeNetId() {
	if (!_freeNetworkVec.empty()) {
		network_t ret = _freeNetworkVec.back();
		_freeNetworkVec.pop_back();
		return ret;
	}
	return _networkIdMax++;
}

void NetworkManager::_freeNetId(network_t id) {
	_freeNetworkVec.push_back(id);
}

NetworkManager* NetworkManager::getInstance() {
	NetworkManager* tmp = instance.load(std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_acquire);
	if (tmp == nullptr) {
		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);
		tmp = instance.load(std::memory_order_relaxed);
		if (tmp == nullptr) {
			tmp = new NetworkManager();
			std::atomic_thread_fence(std::memory_order_release);
			instance.store(tmp, std::memory_order_relaxed);
		}
	}
	return tmp;
}
