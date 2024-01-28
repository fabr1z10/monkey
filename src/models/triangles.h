#pragma once

#include "../model.h"
#include "../components/batchrenderer.h"
#include "../batch/trianglebatch.h"


class TrianglesModel : public Model {
public:
	struct Tri {
		glm::vec3 P0;
		glm::vec3 P1;
		glm::vec3 P2;
		glm::vec4 color;
	};
	TrianglesModel(const std::string& batchId) : Model(), _batchId(batchId) {}
	explicit TrianglesModel(const pybind11::kwargs&);
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
	int getTriangleCount() const;
	const Tri& getTriangle(int) const;
	void init(const glm::vec4& color, const std::vector<float>& data);

private:
    std::string _batchId;
	int _triangleCount;
	std::vector<Tri> _triangles;

};

inline int TrianglesModel::getTriangleCount() const {
	return _triangleCount;
}

inline const TrianglesModel::Tri & TrianglesModel::getTriangle(int i) const {
	return _triangles[i];
}

class TrianglesModelRenderer : public BatchRenderer<TriangleBatch> {
public:
	explicit TrianglesModelRenderer(const std::string&);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs&) override;
	std::type_index getType() override;
	void update(double) override;
private:
	std::shared_ptr<TrianglesModel> _lineModel;
};

