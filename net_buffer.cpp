#include "net_buffer.h"
#include "net_log.h"
#include "stdlib.h"


NetBuffer::NetBuffer()
	:_begin(0),
	_end(0),
	_cap(0),
	_buffer(nullptr) {

}

NetBuffer::~NetBuffer() {
	if (_buffer != nullptr) {
		delete[]_buffer;
	}
}

void NetBuffer::init(size_t size) {
	_buffer = new uint8_t[size];
	_cap = size;
}

bool NetBuffer::write(void* data, size_t size) {
	if (size == 0){
		//net_log_error("write size is 0\n");
		return true;
	}
	if (_begin <= _end) {
		if (_cap - _end + _begin <= size) {
			net_log_error("size is long! size = %d begin=%d end=%d cap = %d\n", size, _begin, _end, _cap);
			return false;
		}
		size_t maxLen = size < (_cap - _end) ? size : (_cap - _end);
		memcpy(_buffer + _end, data, maxLen);

		if (maxLen < size) {
			memcpy(_buffer, (uint8_t*)data + maxLen, size - maxLen);
		}
	} else {
		if (_begin - _end <= size) {
			net_log_error("size is long! size = %d begin=%d end=%d cap = %d\n", size, _begin, _end, _cap);
			return false;
		}
		memcpy(_buffer + _end, data, size);
	}
	_end = (_end + size) > _cap ? (_end + size - _cap) : (_end + size);
	return true;
}

uint8_t* NetBuffer::pickRead(size_t* outSize) {
	if (_end >= _begin) {
		*outSize = _end - _begin;
	} else {
		*outSize = _cap - _begin;
	}
	return _buffer + _begin;
}

uint8_t* NetBuffer::pickWrite(size_t* outSize) {
	if (_begin <= _end) {
		*outSize = _cap - _end;
		if (_begin == 0) {
			*outSize--;
		}
	} else {
		*outSize = _begin - _end - 1;
	}
	return _buffer + _end;
}

void NetBuffer::writeLen(int len) {
	_end += len;
	_end %= _cap;
}

void NetBuffer::readLen(size_t len) {
	if (len > _cap - 1) {
		net_log_error("remove long! len = %d begin=%d end=%d cap = %d\n", len, _begin, _end, _cap);
		return;
	}
	if (_begin < _end) {
		_begin += len;

		if (_begin > _end) {
			net_log_error("remove long!! len = %d begin=%d end=%d cap = %d\n", len, _begin, _end, _cap);
			_begin = _end = 0;
			return;
		}
	} else {
		_begin += len;
		if (_begin >= _cap) {
			_begin -= _cap;

			if (_begin > _end) {
				net_log_error("remove long!!! len = %d begin=%d end=%d cap = %d\n", len, _begin, _end, _cap);
				_begin = _end = 0;
				return;
			}
		}
	}
}

bool NetBuffer::empty() {
	return _begin == _end;
}

size_t NetBuffer::length() {
	if (_begin <= _end) {
		return _end - _begin;
	} else {
		return _cap - _begin + _end;
	}
}

void _copyTo(uint8_t*dst, uint8_t*src, size_t startPos, size_t endPos, size_t cap, size_t size) {
	if (startPos < endPos) {
		memcpy(dst, src + startPos, size);
	} else {
		if (cap - startPos > size) {
			memcpy(dst, src + startPos, size);
		} else {
			memcpy(dst, src + startPos, cap - startPos);
			memcpy(dst, src, size + startPos - cap);
		}
	}
}

bool NetBuffer::copyTo(uint8_t*buf, size_t startPos, size_t size) {
	if (startPos + size > length()){
		return false;
	}
	size_t begin = (startPos + _begin) % _cap;

	_copyTo(buf, _buffer, begin, _end, _cap, size);
	return true;
}

bool NetBuffer::copyTo(uint8_t* buf, size_t size) {
	if (size > length()) {
		return false;
	}

	_copyTo(buf, _buffer, _begin, _end, _cap, size);
	return true;
}

void NetBuffer::copyTo(uint8_t* buf) {
	if (_begin < _end) {
		memcpy(buf, _buffer + _begin, _end - _begin);
	} else {
		memcpy(buf, _buffer + _begin, _cap - _begin);
		memcpy(buf, _buffer, _end);
	}
}

void NetBuffer::clear() {
	_begin = _end = 0;
}
