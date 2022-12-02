#include "net_parse_engine_default.h"
#include "net_buffer.h"
#include "net_conn.h"
#include "net_network.h"

bool NetParseEngineDefault::procRecv(NetConnection*conn, NetBuffer& recvBuffer) {
	NetMsgHeader header;
	size_t headerSize = sizeof(NetMsgHeader);
	if (headerSize > recvBuffer.length()) {
		return false;
	}

	recvBuffer.copyTo((uint8_t*)&header, headerSize);


	if (header.size + headerSize > recvBuffer.length()) {
		return false;
	}
	recvBuffer.readLen(headerSize);

	net_msg_s msg;
	msg.data = new uint8_t[header.size];
	recvBuffer.copyTo(msg.data, header.size);
	msg.size = header.size;
	msg.conn_id = conn->getConnId();
	msg.type = NET_MSG_DATA;
	conn->getNetwork()->pushMsg(msg);

	recvBuffer.readLen(header.size);
	return true;
}

void NetParseEngineDefault::onConnCreate(NetConnection* conn) {
	net_msg_s msg;
	msg.conn_id = conn->getConnId();
	msg.type = NET_MSG_CONNECTED;
	msg.data = nullptr;
	msg.size = 0;
	conn->getNetwork()->pushMsg(msg);
}

bool NetParseEngineDefault::write(NetConnection* conn, NetBuffer& sendBuffer, void* data, size_t size) {
	return false;
}
