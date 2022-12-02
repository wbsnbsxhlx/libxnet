#ifndef net_msg_queue_h__
#define net_msg_queue_h__
#include "libxnet.h"
#include <queue>
#include <mutex>

class NetMsgQueue {
public:
	NetMsgQueue();
	~NetMsgQueue();
	void pushMsg(net_msg_s& msg);
	bool popMsg(net_msg_s& msg);
	void clearQueueMsgs();

private:
	std::queue<net_msg_s> _queueMsgs;
	std::mutex _msgQueueLock;
};

#endif // net_msg_queue_h__
