#pragma once

#include <string>

class File {
public:

	explicit File(const std::string& filename);
	~File();
	uint32_t getLength() const;
	void dumpHex(uint32_t startOffset = 0) const;
	unsigned char* _data;
	uint8_t readByte();
	uint16_t read16BE();

	unsigned char* getPtr();
	void setPtr(unsigned char*);
	bool eof() const;
	// sets the file cursor
	void seek(uint32_t pos);
	[[nodiscard]] uint32_t tell() const;
private:
	uint32_t _pos;
	uint32_t _length;
};

inline bool File::eof() const {
	return _pos >= _length;
}

inline void File::seek(uint32_t pos) {
	_pos = pos;
}

inline uint32_t File::tell() const {
	return _pos;
}
//inline unsigned char * File::getPtr() {
//	return _ptr;
//}
//
//inline void File::setPtr(unsigned char* p) {
//	_ptr = p;
//}

inline uint32_t File::getLength() const {
	return _length;
}