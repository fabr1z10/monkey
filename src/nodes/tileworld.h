#pragma once


#include "../node.h"
#include "../file.h"
#include "../hashpair.h"
#include <stack>
#include "../models/iquad.h"

const float eps = 0.1f;

class Tile {
public:
	//virtual std::vector<float> getCoordinates() = 0;
	Tile(float tileSize) : _tileSize(tileSize) {}
	// returns whether going from x1 to x2 @ height y incurs in a collision.
	// returns a negative number if no collision, otherwise the max distance that can be traveled before collision
	virtual float collideX(float x1, float x2, float y) = 0;

	virtual float collideY(float y1, float y2, float x) = 0;
protected:
	float _tileSize;
};

class SolidTile : public Tile {
public:
	SolidTile(float size) : Tile(size) {}

	float collideX(float x1, float x2, float y) override;

	float collideY(float y1, float y2, float x) override;

};

struct StackInfo {
	StackInfo () : pos(0), rx(0), ry(0), cx(0), cy(0) {}

	uint32_t pos;
	int x, y;
	// repeat x, y
	int rx, ry;
	int cx, cy;
	// step x, y
	int sx, sy;
};

struct FileStatus {
	// current cursor
	FileStatus() : x(0), y(0), fliph(false), flipv(false) {}
	int x;
	int y;
	std::stack<StackInfo> loopStack;
	bool fliph;
	bool flipv;
};

class TileWorld : public Node {
public:
	TileWorld(int tileSize, int width, int height,
		   const std::string& description, const std::string& batchId, const pybind11::kwargs& args);

	float getTileSize() const;

	Tile* getTile(int ix, int iy);

private:
	//void repeat
	void load(File* f, uint32_t offset, uint32_t length, FileStatus& fs);
	//std::unordered_map<uint8_t, std::function<void(File*)>> _funcs;
	int _tileSize;
	int _width;
	int _height;
	int _x, _y;
	std::unordered_map<std::pair<int, int>, std::unique_ptr<Tile>> _tiles;
	int _tilesPerSide;
	std::shared_ptr<IQuads> _model;
	std::unique_ptr<File> _gfx;
	uint16_t _startOfDataSection;
};

inline float TileWorld::getTileSize() const {
	return _tileSize;
}