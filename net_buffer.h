#ifndef net_buffer_h__
#define net_buffer_h__

#include "libxnet.h"

class NetBuffer{
public:	
	NetBuffer();
	~NetBuffer();
	void init(size_t size);
	bool write(void* data, size_t size);

	bool copyTo(uint8_t* buf, size_t size);
	void copyTo(uint8_t* buf);

	uint8_t* pickRead(size_t* outSize);
	uint8_t* pickWrite(size_t* outSize);
	void writeLen(int len);
	void readLen(size_t len);
	bool empty();

	size_t length();
	void clear();
private:

	uint8_t* _buffer;
	size_t _begin;
	size_t _end;
	size_t _cap;
};

#endif // net_buffer_h__
