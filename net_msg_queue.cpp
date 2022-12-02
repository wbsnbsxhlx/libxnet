#include "net_msg_queue.h"

NetMsgQueue::NetMsgQueue() {

}

NetMsgQueue::~NetMsgQueue() {
	clearQueueMsgs();
}

void NetMsgQueue::pushMsg(net_msg_s& msg) {
	std::lock_guard<std::mutex> l(this->_msgQueueLock);
	_queueMsgs.push(msg);
}

bool NetMsgQueue::popMsg(net_msg_s& msg) {
	std::lock_guard<std::mutex> l(_msgQueueLock);
	if (_queueMsgs.size() == 0) {
		return false;
	}
	msg = _queueMsgs.front();
	_queueMsgs.pop();
	return true;
}

void NetMsgQueue::clearQueueMsgs() {
	std::lock_guard<std::mutex> l(this->_msgQueueLock);
	while (!_queueMsgs.empty()) {
		net_free_msg(&_queueMsgs.front());
		_queueMsgs.pop();
	}
}
