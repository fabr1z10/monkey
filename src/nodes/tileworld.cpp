#include "tileworld.h"
#include "../models/lines.h"
#include "../pyhelper.h"
#include <iostream>
#include <stack>


using namespace pybind11::literals; // to bring in the `_a` literal

float SolidTile::collideX(float x1, float x2, float y) {
	if (x1 < 0 && x2 >= 0) {
		return (-x1 - eps);
	}
	if (x2 > _tileSize && x1 <= _tileSize) {
		return (_tileSize + eps - x1);
	}
}

float SolidTile::collideY(float y1, float y2, float x) {
	if (y1 < 0 && y2 >= 0) {
		return (-y1 - 0.1f);
	}
	if (y2 > _tileSize && y1 <= _tileSize) {
		return (_tileSize - 0.1f);
	}
}

Tile * TileWorld::getTile(int ix, int iy) {
	auto it = _tiles.find(std::make_pair(ix, iy));
	if (it == _tiles.end()) {
		return nullptr;
	}
	return it->second.get();
}



void TileWorld::load(File *f, uint32_t offset, uint32_t length, FileStatus& fs) {
	float k = 1.0f / _tilesPerSide;

	f->seek(offset);
	while (f->tell() < offset + length) {
		auto tileGeom = f->readByte();
		if (tileGeom == 0xFF) {
			continue;
		}
		// if the MSB is set, this is a command
		if (tileGeom & 0x80) {
			auto opcode = tileGeom & 0x7F;
			if (opcode == 0x01) {
				// REPEAT X
				StackInfo info;
				info.rx = f->readByte();
				info.sx = f->readByte();
				info.pos = f->tell();
				info.ry = 1;
				info.cx = 0;
				info.cy = 0;
				info.sy = 0;
				info.x = _x;
				info.y = _y;
				fs.loopStack.push(info);
			} else if (opcode == 0x02) {
				// ENDLOOP
				if (fs.loopStack.empty()) {
					std::cerr << "Error: found ENDLOOP without STARTLOOP";
					exit(1);
				}
				auto& p = fs.loopStack.top();
				if (p.cx < p.rx-1) {
					p.cx ++;
					_x = p.x + p.cx * p.sx;
                    f->seek(p.pos);
				} else {
					p.cy ++;
					p.cx = 0;
					if (p.cy >=p.ry) {
					    fs.loopStack.pop();
					    _x = p.x + p.rx * p.sx;
                        _y = p.y;
					} else {
                        _x = p.x;
                        _y = p.y + p.cy * p.sy;
                        f->seek(p.pos);
                    }
				}


			} else if (opcode == 0x03) {
                // REPEAT XY
                StackInfo info;
                info.rx = f->readByte();
                info.sx = f->readByte();
                info.ry = f->readByte();
                info.cx = 0;
                info.cy = 0;
                info.sy = f->readByte();
                info.pos = f->tell();
                info.x = _x;
                info.y = _y;
                fs.loopStack.push(info);
			} else if (opcode == 0x04) {
				fs.fliph=true;
			} else if (opcode == 0x05) {
				fs.flipv = true;
			} else if (opcode == 0x06) {
//				// MOVE ABSOLUTE
                _x = f->readByte();
                _y = f->readByte();
			} else if (opcode == 0x07) {
//				// MOVE RELATIVE
				auto dx = static_cast<signed char>(f->readByte());
				auto dy = static_cast<signed char>(f->readByte());
				_x += dx;
				_y += dy;
			} else if (opcode == 0x08) {
				// LOAD FROM EXTERNAL
				int id = f->readByte();
				_gfx->seek(2 + id * 4);
				auto offset = _gfx->read16BE();
				auto length = _gfx->read16BE();
				load(_gfx.get(), _startOfDataSection + offset, length, fs);


			}


		} else {
			// tile info
			int tileId = f->readByte();
			int col = tileId / _tilesPerSide;
			int row = tileId - col * _tilesPerSide;

			std::cout << "id: " << tileId << ", geom: " << tileGeom << "\n";
			if (tileId != 0xFF) {
				auto kwargs = pybind11::dict("pos"_a = glm::vec3(_x * _tileSize, _y * _tileSize, 0.f),
											 "size"_a = glm::vec2(_tileSize, _tileSize), "normalized"_a = true,
											 "pal"_a = 0, "fliph"_a=fs.fliph, "flipv"_a=fs.flipv);
				_model->addQuad(glm::vec4(row * k, col * k, k, k), kwargs);
				fs.fliph = false;
				fs.flipv = false;

			}

			if (tileGeom == 0x01) {
				// SOLID
				_tiles[{_x, _y}] = std::make_unique<SolidTile>(_tileSize);

			}

			_x++;
			if (_x >= _width) {
				_x = 0;
				_y++;
			}

		}

	}
}



TileWorld::TileWorld(int tileSize, int width, int height, const std::string& file, const std::string &batchId,
	const pybind11::kwargs& args) : Node(), _x(0), _y(0), _tileSize(tileSize), _width(width), _height(height) {


	auto gfxFile = py_get_dict<std::string>(args, "gfx", "");
	if (gfxFile != "") {
		_gfx = std::make_unique<File>(gfxFile);
		_startOfDataSection = _gfx->read16BE();

	}
	//_funcs[0x01] = [this](File* f) {};

	File f(file);
	_model = std::make_shared<IQuads>(batchId);

	int blocks = f.getLength() / 3;


	// if n bits for each tile -> 2**(n/2) tiles per side
	_tilesPerSide = 1 << 5;

	FileStatus fs;
	load(&f, 0, f.getLength(), fs);


	setModel(_model, pybind11::dict("batch"_a=batchId));

//
//		auto c = description[i];
//		int repeat {1};
//
//		if (i < description.size() && isdigit(description[i+1])) {
//			int j = i+2;
//			while (j<description.size() && isdigit(description[j])) j++;
//			repeat = std::stoi(description.substr(i+1, j-1-i));
//		}
//
//		if (c == '.') {
//			// empty.
//
//		} else if c=='' {
//
//		}
//




}

