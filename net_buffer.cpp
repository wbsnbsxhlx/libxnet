#include "net_buffer.h"
#include "net_log.h"
#include "stdlib.h"

NetBuffer::NetBuffer()
	:_begin(0),
	_end(0),
	_cap(0),
	_buffer(nullptr)
{

}

void NetBuffer::init(size_t size)
{
	_buffer = new uint8_t[size];
	_cap = size;
}

bool NetBuffer::write(void* data, size_t size)
{
	if (_begin < _end)
	{
		if (_cap - _end + _begin <= size){
			log(LOG_ERROR, "size is long! size = %d cap = %d", size, _cap);
			return false;
		}
		size_t maxLen = size < (_cap-_end)?size:(_cap-_end);
		memcpy(_buffer + _end, data, maxLen);
		
		if (maxLen < size){
			memcpy(_buffer, (uint8_t*)data + maxLen, size - maxLen);
		}
	}else{
		if (_begin - _end <= size){
			log(LOG_ERROR, "size is long! size = %d cap = %d", size, _cap);
			return false;
		}
		memcpy(_buffer + _end, data, size);
	}
	_end += (_end+size) > _cap ? (_end+size-_cap) : (_end+size);
	return true;
}

uint8_t* NetBuffer::pick(size_t& outSize)
{
	if (_end > _begin){
		outSize = _end - _begin;
	}else{
		outSize = _cap - _begin;
	}
	return _buffer + _begin;
}

void NetBuffer::remove(size_t len)
{
	if (len > _cap-1){
		log(LOG_ERROR, "remove long! cap=%d len=%d", _cap, len);
		return;
	}
	if (_begin < _end){
		_begin += len;

		if (_begin > _end){
			log(LOG_ERROR, "remove long! cap=%d len=%d", _cap, len);
			_begin = _end = 0;
			return;
		}
	}else{
		_begin += len;
		if (_begin >= _cap){
			_begin -= _cap;

			if (_begin > _end){
				log(LOG_ERROR, "remove long! cap=%d len=%d", _cap, len);
				_begin = _end = 0;
				return;
			}
		}
	}
}

bool NetBuffer::empty()
{
	return _begin == _end;
}

