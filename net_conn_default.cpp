#include "net_conn_default.h"
#include "net_log.h"
#include <assert.h>

bool NetConnectionDefault::onProcRecv() {
	NetMsgHeader header;
	size_t headerSize = sizeof(NetMsgHeader);
	while (true) {
		if (headerSize > _recvBuffer.length()) {
			return true;
		}

		_recvBuffer.copyTo((uint8_t*)&header, headerSize);

		if (header.size > _recvBuffer.cap()) {
			net_log_error("msg size is too long %d\n", header.size);
			return false;
		}
		if (header.size + headerSize > _recvBuffer.length()) {
			return true;
		}
		_recvBuffer.readLen(headerSize);

		net_msg_s msg;
		msg.data = new uint8_t[header.size];
		_recvBuffer.copyTo(msg.data, header.size);
		msg.size = header.size;
		msg.conn_id = getConnId();
		msg.type = NET_MSG_DATA;
		getNetwork()->pushMsg(msg);

		_recvBuffer.readLen(header.size);
	}

	return true;
}

void NetConnectionDefault::onConnCreate() {
	net_msg_s msg;
	msg.conn_id = getConnId();
	msg.type = NET_MSG_CONNECTED;
	msg.data = nullptr;
	msg.size = 0;
	getNetwork()->pushMsg(msg);
}

bool NetConnectionDefault::onWrite(void* data, size_t size) {
	NetConnectionDefault::NetMsgHeader header;
	header.size = htons(((uint16_t)size));

	_sendBuffer.write(&header, sizeof(header));
	
	return true;
}

