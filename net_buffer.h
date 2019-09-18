#ifndef net_buffer_h__
#define net_buffer_h__

#include "libxnet.h"

class NetBuffer{
public:	
	NetBuffer();
	void init(size_t size);
	bool write(void* data, size_t size);

	uint8_t* pick(size_t& outSize);
	void remove(size_t len);
private:

	uint8_t* _buffer;
	size_t _begin;
	size_t _end;
	size_t _cap;
};

#endif // net_buffer_h__
