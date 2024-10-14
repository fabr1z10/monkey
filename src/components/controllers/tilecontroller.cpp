#include "tilecontroller.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../node.h"
#include "../../nodes/tileworld.h"
#include "../../pyhelper.h"
#include "../../models/modelmake.h"
#include "../../shapes/aabb.h"

extern GLFWwindow * window;

TileController::TileController(float speed, float width, float height, const pybind11::kwargs& args) : _speed(speed),
	_width(width), _height(height), Component(args) {
	_batchId = py_get_dict<std::string>(args, "batch", "");
	_hw = _width * 0.5f;

}


void TileController::start() {
	_world = dynamic_cast<TileWorld*>(m_node->getParent());

	if (_debugNode != nullptr) {
		_debugNode->remove();
	}
	auto shape = std::make_shared<AABB>(-_hw, _hw, 0.f, _height);
	auto& modelMaker = ModelMaker::instance();
	auto model = modelMaker.make(_batchId, shape, glm::vec4(1.f), FillType::OUTLINE);
	auto node = std::make_shared<Node>();
	node->setModel(model);//pybind11::dict("batch"_a = _batchId));
	m_node->add(node);
	_debugNode = node.get();
}

void TileController::testHorizontal(float& dx) {
	glm::vec2 p = m_node->getWorldPosition();

	float x0, x1, y[2];
	y[0] = p.y;
	y[1] = p.y + _height;
	if (dx > 0) {
		x0 = p.x + _hw;
	} else {
		x0 = p.x - _hw;
	}

	int i0 = static_cast<int>(x0 / _world->getTileSize());
	int i1 = static_cast<int>(x1 / _world->getTileSize());
	for (float h : y) {
		int j = static_cast<int>(h / _world->getTileSize());
		for (int i = i0; i <= i1; ++i) {
			float xOrig = i * _world->getTileSize();
			float yOrig = j * _world->getTileSize();
			auto* tile = _world->getTile(i, j);
			if (tile != nullptr) {
				auto length = tile->collideX(x0 - xOrig, x1 - xOrig, h - yOrig);
				if (length > 0) {
					dx = length;
					x1 = x0 + length;
					break;
				}
			}
		}
	}
}

void TileController::update(double dt) {
	bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	bool down = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;

	float dx = right ? 1.0f : (left ? -1.0f : 0.f);
	float dy = up ? 1.0f : (down ? -1.0f : 0.0f);

	glm::vec2 delta(dx, dy);

	delta *= _speed*dt;
	if (dx != 0.f || dy != 0.f) {
		//if (dx != 0.f) testHorizontal(dx);
		// if I go right, I need to check bottom right and top right


	}







	m_node->move(glm::vec3(delta.x, delta.y, 0.f));

}