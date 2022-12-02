#include "net_parse_engine_websocket.h"
#include "net_buffer.h"
#include "libxnet.h"
#include "net_conn.h"

#include <stdio.h>
#include <string.h>

#include <stdint.h>

typedef struct {
	uint32_t state[5];
	uint32_t count[2];
	unsigned char buffer[64];
} SHA1_CTX;

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

static const char base64en[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/',
};

static const unsigned char base64de[] = {
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 62, 255, 255, 255, 63,
	52, 53, 54, 55, 56, 57, 58, 59,
	60, 61, 255, 255, 255, 255, 255, 255,
	255, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22,
	23, 24, 25, 255, 255, 255, 255, 255,
	255, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 255, 255, 255, 255, 255
};

void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]) {
	uint32_t a, b, c, d, e;

	typedef union {
		unsigned char c[64];
		uint32_t l[16];
	} CHAR64LONG16;

	CHAR64LONG16 block[1];
	memcpy(block, buffer, 64);

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	R0(a, b, c, d, e, 0);
	R0(e, a, b, c, d, 1);
	R0(d, e, a, b, c, 2);
	R0(c, d, e, a, b, 3);
	R0(b, c, d, e, a, 4);
	R0(a, b, c, d, e, 5);
	R0(e, a, b, c, d, 6);
	R0(d, e, a, b, c, 7);
	R0(c, d, e, a, b, 8);
	R0(b, c, d, e, a, 9);
	R0(a, b, c, d, e, 10);
	R0(e, a, b, c, d, 11);
	R0(d, e, a, b, c, 12);
	R0(c, d, e, a, b, 13);
	R0(b, c, d, e, a, 14);
	R0(a, b, c, d, e, 15);
	R1(e, a, b, c, d, 16);
	R1(d, e, a, b, c, 17);
	R1(c, d, e, a, b, 18);
	R1(b, c, d, e, a, 19);
	R2(a, b, c, d, e, 20);
	R2(e, a, b, c, d, 21);
	R2(d, e, a, b, c, 22);
	R2(c, d, e, a, b, 23);
	R2(b, c, d, e, a, 24);
	R2(a, b, c, d, e, 25);
	R2(e, a, b, c, d, 26);
	R2(d, e, a, b, c, 27);
	R2(c, d, e, a, b, 28);
	R2(b, c, d, e, a, 29);
	R2(a, b, c, d, e, 30);
	R2(e, a, b, c, d, 31);
	R2(d, e, a, b, c, 32);
	R2(c, d, e, a, b, 33);
	R2(b, c, d, e, a, 34);
	R2(a, b, c, d, e, 35);
	R2(e, a, b, c, d, 36);
	R2(d, e, a, b, c, 37);
	R2(c, d, e, a, b, 38);
	R2(b, c, d, e, a, 39);
	R3(a, b, c, d, e, 40);
	R3(e, a, b, c, d, 41);
	R3(d, e, a, b, c, 42);
	R3(c, d, e, a, b, 43);
	R3(b, c, d, e, a, 44);
	R3(a, b, c, d, e, 45);
	R3(e, a, b, c, d, 46);
	R3(d, e, a, b, c, 47);
	R3(c, d, e, a, b, 48);
	R3(b, c, d, e, a, 49);
	R3(a, b, c, d, e, 50);
	R3(e, a, b, c, d, 51);
	R3(d, e, a, b, c, 52);
	R3(c, d, e, a, b, 53);
	R3(b, c, d, e, a, 54);
	R3(a, b, c, d, e, 55);
	R3(e, a, b, c, d, 56);
	R3(d, e, a, b, c, 57);
	R3(c, d, e, a, b, 58);
	R3(b, c, d, e, a, 59);
	R4(a, b, c, d, e, 60);
	R4(e, a, b, c, d, 61);
	R4(d, e, a, b, c, 62);
	R4(c, d, e, a, b, 63);
	R4(b, c, d, e, a, 64);
	R4(a, b, c, d, e, 65);
	R4(e, a, b, c, d, 66);
	R4(d, e, a, b, c, 67);
	R4(c, d, e, a, b, 68);
	R4(b, c, d, e, a, 69);
	R4(a, b, c, d, e, 70);
	R4(e, a, b, c, d, 71);
	R4(d, e, a, b, c, 72);
	R4(c, d, e, a, b, 73);
	R4(b, c, d, e, a, 74);
	R4(a, b, c, d, e, 75);
	R4(e, a, b, c, d, 76);
	R4(d, e, a, b, c, 77);
	R4(c, d, e, a, b, 78);
	R4(b, c, d, e, a, 79);
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	a = b = c = d = e = 0;
	memset(block, '\0', sizeof(block));
}

void SHA1Init(SHA1_CTX * context) {
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
	context->count[0] = context->count[1] = 0;
}

void SHA1Update(SHA1_CTX * context, const unsigned char *data, uint32_t len) {
	uint32_t i;

	uint32_t j;

	j = context->count[0];
	if ((context->count[0] += len << 3) < j)
		context->count[1]++;
	context->count[1] += (len >> 29);
	j = (j >> 3) & 63;
	if ((j + len) > 63) {
		memcpy(&context->buffer[j], data, (i = 64 - j));
		SHA1Transform(context->state, context->buffer);
		for (; i + 63 < len; i += 64) {
			SHA1Transform(context->state, &data[i]);
		}
		j = 0;
	} else
		i = 0;
	memcpy(&context->buffer[j], &data[i], len - i);
}

void SHA1Final(unsigned char digest[20], SHA1_CTX * context) {
	unsigned i;
	unsigned char finalcount[8];
	unsigned char c;

	for (i = 0; i < 8; i++) {
		finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
	}

	c = 0200;
	SHA1Update(context, &c, 1);
	while ((context->count[0] & 504) != 448) {
		c = 0000;
		SHA1Update(context, &c, 1);
	}
	SHA1Update(context, finalcount, 8); 
	for (i = 0; i < 20; i++) {
		digest[i] = (unsigned char)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
	}
	memset(context, '\0', sizeof(*context));
	memset(&finalcount, '\0', sizeof(finalcount));
}

void SHA1(char *hash_out, const char *str, int len) {
	SHA1_CTX ctx;
	unsigned int ii;

	SHA1Init(&ctx);
	for (ii = 0; ii<len; ii += 1)
		SHA1Update(&ctx, (const unsigned char*)str + ii, 1);
	SHA1Final((unsigned char *)hash_out, &ctx);
}

unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out) {
	int s;
	unsigned int i;
	unsigned int j;
	unsigned char c;
	unsigned char l;

	s = 0;
	l = 0;
	for (i = j = 0; i < inlen; i++) {
		c = in[i];

		switch (s) {
		case 0:
			s = 1;
			out[j++] = base64en[(c >> 2) & 0x3F];
			break;
		case 1:
			s = 2;
			out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
			break;
		case 2:
			s = 0;
			out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
			out[j++] = base64en[c & 0x3F];
			break;
		}
		l = c;
	}

	switch (s) {
	case 1:
		out[j++] = base64en[(l & 0x3) << 4];
		out[j++] = '=';
		out[j++] = '=';
		break;
	case 2:
		out[j++] = base64en[(l & 0xF) << 2];
		out[j++] = '=';
		break;
	}

	out[j] = 0;

	return j;
}

unsigned int base64_decode(const char *in, unsigned int inlen, unsigned char *out) {
	unsigned int i;
	unsigned int j;
	unsigned char c;

	if (inlen & 0x3) {
		return 0;
	}

	for (i = j = 0; i < inlen; i++) {
		if (in[i] == '=') {
			break;
		}
		if (in[i] < '+' || in[i] > 'z') {
			return 0;
		}

		c = base64de[(unsigned char)in[i]];
		if (c == 255) {
			return 0;
		}

		switch (i & 0x3) {
		case 0:
			out[j] = (c << 2) & 0xFF;
			break;
		case 1:
			out[j++] |= (c >> 4) & 0x3;
			out[j] = (c & 0xF) << 4;
			break;
		case 2:
			out[j++] |= (c >> 2) & 0xF;
			out[j] = (c & 0x3) << 6;
			break;
		case 3:
			out[j++] |= c;
			break;
		}
	}

	return j;
}


NetParseEngineWebsocket::NetParseEngineWebsocket()
	:state(WS_WAITING_HANDSHAKE)
{

}

NetParseEngineWebsocket::~NetParseEngineWebsocket() {

}

void NetParseEngineWebsocket::onConnCreate(NetConnection*conn) {}

bool splitKeyValue(const uint8_t* line, size_t size, const uint8_t** key, const uint8_t** value) {
	for (size_t i = 0; i < size; i++) {
		if (line[i] == ' ') {
			*key = line;
			*value = line+i+1;
			return true;
		}
	}

	return false;
}

bool readLine(uint8_t *buf, uint32_t *size) {
	uint8_t *end = (uint8_t *)strstr((char *)buf, "\r\n");
	if (end == nullptr){
		return false;
	}

	*size = end - buf;
	
	return true;
}

int NetParseEngineWebsocket::_procHandshake(NetBuffer& recvBuffer, char *outBuf, size_t *outSize) {
	if (recvBuffer.length() > 2048){
		return -1;
	}
	uint8_t header[2048];
	recvBuffer.copyTo(header);
	char *p = strstr((char *)header, "\r\n\r\n");
	if (p == nullptr){
		return 0;
	}
	*(p+2) = 0;
	recvBuffer.readLen(p - (char *)header + 4);

	printf("src:\n%s\n", (char *)header);

	size_t size;
	uint8_t* line = header;
	const uint8_t* key = NULL;
	const uint8_t* value = NULL;
	int fixedNeed = 0;
	*outSize = 0;
	while (readLine(line, &size)) {
		if (!splitKeyValue(line, size, &key, &value)){
			return -1;
		}
		switch (*(uint32_t*)key) {
			case 0x2d636553:
				if (*(key + 13) != '-') {
					return -1;
				}
				if (*(uint32_t*)(key + 14) == 0x3a79654b) {// Sec-WebSocket-Key:
					if (*outSize == 0) return -1;
					size_t valueLen = size - (value-key);
					if (valueLen >= 40){
						return -1;
					}
					char websocketKey[40];
					memcpy(websocketKey, value, valueLen);
					websocketKey[valueLen] = 0;
					char srcKey[128];
					int n = sprintf_s(srcKey, "%s%s", websocketKey, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					char sha1[20];
					SHA1(sha1, srcKey, strlen(srcKey));
					char base64[40];
					base64_encode((const unsigned char*)sha1, 20, base64);

					*outSize += sprintf_s(outBuf + *outSize, 1024 - *outSize, "%s%s\r\n", "Sec-WebSocket-Accept: ", base64);
					fixedNeed++;

				} else if (*(uint32_t*)(key + 14) == 0x746f7250) {// Sec-WebSocket-Protocol:
					if (*outSize == 0) return -1;
					memcpy(outBuf + *outSize, line, size + 2);
				}
				break;
			case 0x20544547:// GET /WebSocket/LiveVideo HTTP/1.1
				if (*outSize != 0) return -1;
				*outSize += sprintf_s(outBuf + *outSize, 1024 - *outSize, "%s\r\n", "HTTP/1.1 101 Switching Protocols");
				fixedNeed++;
				break;
			case 0x72677055:// Upgrade: WebSocket
				if (*outSize == 0) return -1;
				*outSize += sprintf_s(outBuf + *outSize, 1024 - *outSize, "%s\r\n", "Upgrade: websocket");
				fixedNeed++;
				break;
			case 0x6e6e6f43:// Connection: Upgrade
				if (*outSize == 0) return -1;
				*outSize += sprintf_s(outBuf + *outSize, 1024 - *outSize, "%s\r\n", "Connection: Upgrade");
				fixedNeed++;
				break;
		}
		line += size+2;
	}

	if (fixedNeed != 4){
		return -1;
	}

	return 1;
}

bool NetParseEngineWebsocket::procRecv(NetConnection*conn, NetBuffer& recvBuffer) {
	int result;
	switch (state) {
		case NetParseEngineWebsocket::WS_WAITING_HANDSHAKE:
			char outBuf[1024];
			size_t outSize;
			result = _procHandshake(recvBuffer, outBuf, &outSize);
			if (result == 0){
				return false;
			} else if (result == -1) {
				conn->close();
				return false;
			} else if (result == 1) {
				if (outSize == 0){
					conn->close();
					return false;
				}
				outSize += sprintf_s(outBuf + outSize, 1024 - outSize, "\r\n");
				printf("resp:\n%s\n", outBuf);

				conn->write(outBuf, outSize);

				net_msg_s msg;
				msg.conn_id = conn->getConnId();
				msg.type = NET_MSG_CONNECTED;
				msg.data = nullptr;
				msg.size = 0;
				conn->getNetwork()->pushMsg(msg);

				state = WS_OPENED_READHEAD;
				return true;
			}
			break;
		default:
			size_t len;
			uint8_t *header = recvBuffer.pickRead(&len);
			char buffer[9] = {0};
			for (int i = 0; i < len; i++) {
				itoa(header[i], buffer, 2);
				printf("%s,", buffer);
			}
			break;
		//case NetParseEngineWebsocket::WS_OPENED_READHEAD:
		//	break;
		//case NetParseEngineWebsocket::WS_OPENED_READDATA:
		//	break;
		//case NetParseEngineWebsocket::WS_ERROR:
		//	break;
		//default:
		//	break;
	}
	return false;
}

bool writeFrameHead() {
	return true;
}

bool NetParseEngineWebsocket::write(NetConnection* conn, NetBuffer& sendBuffer, void* data, size_t size) {
	if (state != WS_WAITING_HANDSHAKE) {
		if (!writeFrameHead()){
			return false;
		}
	}

	return sendBuffer.write(data, size);
}
