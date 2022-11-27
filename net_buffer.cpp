#include "net_buffer.h"
#include "net_header.h"
#include "net_log.h"
#include "stdlib.h"


NetBuffer::NetBuffer()
	:_begin(0),
	_end(0),
	_cap(0),
	_buffer(nullptr)
{

}

NetBuffer::~NetBuffer()
{
	if (_buffer != nullptr)
	{
		delete []_buffer;
	}
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

uint8_t* NetBuffer::pickRead(size_t& outSize)
{
	if (_end > _begin){
		outSize = _end - _begin;
	}else{
		outSize = _cap - _begin;
	}
	return _buffer + _begin;
}

uint8_t* NetBuffer::pickWrite(size_t& outSize)
{
	if (_begin <= _end){
		outSize = _cap-_end;
		if (_begin == 0){
			outSize--;
		}
	}else{
		outSize = _begin-_end-1;
	}
	return _buffer + _end;
}

void NetBuffer::writeLen(int len)
{
	_end += len;
	_end %= _cap;
	if (_end >= _begin){
		log(LOG_ERROR, "write len error");
	}
}

void NetBuffer::readLen(size_t len)
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

size_t NetBuffer::length()
{
	if (_begin <= _end){
		return _end-_begin;
	}else{
		return _cap-_begin+_end;
	}
}

void NetBuffer::copyTo(uint8_t* buf)
{
	if(_begin < _end){
		memcpy(buf, _buffer + _begin, _end - _begin);
	}else{
		memcpy(buf, _buffer + _begin, _cap - _begin);
		memcpy(buf, _buffer, _end);
	}
}

bool NetBuffer::makeMsg(net_msg_s& msg)
{
	NetMsgHeader header;
	size_t headerSize = sizeof(NetMsgHeader);
	if (headerSize <= length()){
		return false;
	}

	void* dataPtr = nullptr;
	if (_begin <= _end){
		memcpy(&header, _buffer + _begin, headerSize);
		dataPtr = _buffer + _begin + headerSize;
	}else{
		if (_cap-_begin > headerSize){
			memcpy(&header, _buffer+_begin, headerSize);
			dataPtr = _buffer + _begin + headerSize;
		}else {
			memcpy(&header, _buffer + _begin, _cap - _begin);
			memcpy((uint8_t*)(&header) + _cap - _begin, _buffer, headerSize + _begin - _cap);
			dataPtr = _buffer + headerSize + _begin - _cap;
		}
	}

	if (header.size+headerSize > length()){
		return false;
	}
	msg.data = new uint8_t[header.size];
	memcpy(msg.data, dataPtr, header.size);
	msg.size = header.size;

	return true;
}