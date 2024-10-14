#include "file.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include "assetmanager.h"

File::File(const std::string& file) : _data(nullptr), _pos(0) {
	std::string fullPath = AssetManager::instance().getDirectory() + file;
	std::cout << "fullpath is " << fullPath << "\n";
	std::ifstream stream(fullPath.c_str(), std::ios::in | std::ios::binary);
	if (!stream.good())
	{
		std::cout << "err opening " << file;
		exit(1);
	}
	stream.seekg(0, std::ios::end);
	uint32_t flen = stream.tellg();
	stream.seekg(0, std::ios::beg);
	_length = flen;

	// allocate memory
	auto *mem = (char*)malloc(flen);
	if (mem == nullptr) {
		exit(2);
	}
	stream.read(mem, _length);
	_data = reinterpret_cast<unsigned char*>(mem);

}


File::~File() {
	delete[] _data;
}

void File::dumpHex(uint32_t startOffset) const {
	for (uint32_t i = startOffset; i < _length; i+=16) {
		std::cout << std::setfill('0') << std::setw(8) << std::hex << i << ' ';
		for (uint32_t j = i; j < std::min(i+16, _length); ++j) {
			std::cout << std::setfill('0') << std::setw(2) << std::hex << (int) _data[j] << ' ';
		}
		std::cout << std::endl << std::dec;
	}
}

uint8_t File::readByte() {
	auto b = *(reinterpret_cast<uint8_t*>(_data + _pos));
	_pos ++;
	return b;

}

uint16_t File::read16BE() {
	auto b = reinterpret_cast<uint8_t*>(_data + _pos);
	_pos += 2;
	return (b[0] << 8) | b[1];
}