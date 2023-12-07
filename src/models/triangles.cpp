#include "triangles.h"
#include "../math/earcut.h"

void TrianglesModel::init(const glm::vec4 &color, const std::vector<float> &points) {
	using Coord = float;
	using Point = std::array<Coord, 2>;
	using N = uint32_t;

	std::vector<Point> polygon;
	for (size_t i = 0; i < points.size(); i += 2) {
		size_t offset = i;
		polygon.push_back({points[i], points[i+1]});
	}
	std::vector<std::vector<Point>> p;
	p.push_back(polygon);
	auto tri = mapbox::earcut<N>(p);
	assert(tri.size() % 3==0);
	_triangleCount = tri.size() / 3;
	for (size_t i = 0; i < _triangleCount; ++i) {
		Tri t;
		t.P0 = glm::vec3(p[0][tri[3*i]][0], p[0][tri[3*i]][1], 0);
		t.P1 = glm::vec3(p[0][tri[3*i+1]][0], p[0][tri[3*i+1]][1], 0);
		t.P2 = glm::vec3(p[0][tri[3*i+2]][0], p[0][tri[3*i+2]][1], 0);
		t.color = glm::vec4(1.f);
		_triangles.push_back(t);
	}

}
TrianglesModel::TrianglesModel(const pybind11::kwargs & args) : Model() {
	auto color = args["color"].cast<glm::vec4>();
	auto points = args["points"].cast<std::vector<float>>();
	init(color, points);



}
std::shared_ptr<Renderer> TrianglesModel::getRenderer(const pybind11::kwargs& args) {
	return std::make_shared<TrianglesModelRenderer>(args);
}
TrianglesModelRenderer::TrianglesModelRenderer(const pybind11::kwargs& args) : BatchRenderer<TriangleBatch>(args) {
	//assert(_lineBatch);

}

void TrianglesModelRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs&) {
	_lineModel = std::dynamic_pointer_cast<TrianglesModel>(model);
	auto qc = _lineModel->getTriangleCount();
	for (int i = 0; i < qc; ++i) {
		_primitiveIds.push_back(_batch->getPrimitiveId());
	}

}

std::type_index TrianglesModelRenderer::getType() {
	return std::type_index(typeid(Renderer));
}

void TrianglesModelRenderer::update(double dt) {

	// get world pos
	auto pos = m_node->getWorldMatrix();


	for (size_t i = 0; i < _primitiveIds.size(); ++i) {
		const auto& s = _lineModel->getTriangle(i);
		auto a = glm::vec3(pos * glm::vec4(s.P0, 1.f));
		auto b = glm::vec3(pos * glm::vec4(s.P1, 1.f));
		auto c = glm::vec3(pos * glm::vec4(s.P2, 1.f));
		_batch->setTriangle(_primitiveIds[i], a, b, c, s.color);
	}


}