#pragma once

#include "../../component.h"

class TileWorld;


class TileController : public Component {
public:
	TileController(float speed, float width, float height, const pybind11::kwargs& args);

	void start() override;

	void update(double) override;
private:
	void testHorizontal (float& dx);
	TileWorld* _world;
	float _width;
	float _hw;
	float _height;
	float _speed;
	std::string _batchId;
	Node* _debugNode;

};