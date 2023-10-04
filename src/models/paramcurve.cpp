#include "paramcurve.h"
#include "../pyhelper.h"

ParametricCurve::ParametricCurve(const pybind11::kwargs& args) {
	auto domain = py_get_dict<glm::vec2>(args, "domain");
	_z0 = domain.x;
	_z1 = domain.y;
	_points = py_get_dict<int>(args, "points");
	_func = py_get_dict<pybind11::function>(args, "f");
	// n points -> n-1 steps
	_step = (_z1 - _z0) / (_points -1);
}

std::shared_ptr<Renderer> ParametricCurve::getRenderer(const pybind11::kwargs& args) {
	return std::make_shared<ParametricCurveRenderer>(args);
}

ParametricCurveRenderer::ParametricCurveRenderer(const pybind11::kwargs& args) : BatchRenderer<LineBatch>(args) {
}

void ParametricCurveRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs&) {
	_lineModel = std::dynamic_pointer_cast<ParametricCurve>(model);

	int p = _lineModel->getPoints();
	for (int i = 0; i < p - 1; ++i) {
		_primitiveIds.push_back(_batch->getPrimitiveId());
	}

}

void ParametricCurveRenderer::update(double) {
	float step = _lineModel->getStep();
	auto pos = m_node->getWorldPosition();

	float z = _lineModel->getInitialPoint();
	auto f = _lineModel->getFunction();
	auto previous = f(z).cast<glm::vec3>();
	for (size_t i = 0; i < _primitiveIds.size(); ++i) {
		z += step;
		auto next = f(z).cast<glm::vec3>();
		_batch->setLine(_primitiveIds[i], pos + previous, pos +next, glm::vec4(1.f));
		previous = next;
	}
}

std::type_index ParametricCurveRenderer::getType() {
	return std::type_index(typeid(Renderer));
}


