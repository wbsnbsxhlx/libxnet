#include "net_network_manager.h"
#include "net_network.h"
#include <atomic>
#include <mutex>

std::atomic<NetworkManager*> NetworkManager::instance = nullptr;

NetworkManager::NetworkManager()
	:_networkIdMax(1)
{
	//windows需要初始化链接库
	WSADATA ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
}

NetworkManager::~NetworkManager()
{
	WSACleanup();
}

network_t NetworkManager::createNetwork(int thread_num, int max_client, int recv_buf_size, int send_buf_size)
{
	Network* nw = new Network();
	nw->init(thread_num, max_client, recv_buf_size, send_buf_size);
}

NetworkManager* NetworkManager::getInstance()
{
	NetworkManager* tmp = instance.load(std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_acquire);
	if (tmp == nullptr){
		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);
		tmp = instance.load(std::memory_order_relaxed);
		if (tmp == nullptr)
		{
			tmp = new NetworkManager();
			std::atomic_thread_fence(std::memory_order_release);
			instance.store(tmp, std::memory_order_relaxed);
		}
	}
	return tmp;
}