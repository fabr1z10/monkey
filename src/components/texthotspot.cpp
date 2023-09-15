#include "texthotspot.h"
#include "../shapes/aabb.h"
#include "../node.h"

TextHotSpot::TextHotSpot(const pybind11::kwargs &args) : PyHotSpot(nullptr, args) {

}

std::type_index TextHotSpot::getType() {
	return std::type_index(typeid(HotSpot));
}

void TextHotSpot::start() {
	auto bounds = m_node->getModel()->getBounds();
	_shape = std::make_shared<AABB>(bounds.min.x, bounds.max.x, bounds.min.y, bounds.max.y);
	HotSpot::start();
}