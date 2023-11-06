#pragma once

#include <string>
#include "../shader.h"
#include "../models/skeletal.h"
#include "../components/batchrenderer.h"

class SkeletalRenderer : public BatchRenderer<ProvaBatch> {
public:
    SkeletalRenderer(const pybind11::kwargs& args);
    ~SkeletalRenderer();
    void draw(Shader*) override;
	std::type_index getType() override;
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;

private:
	SkeletalModel* _model;
	std::vector<glm::mat4> _bones;
};

inline std::type_index SkeletalRenderer::getType() {
	return std::type_index(typeid(Renderer));
}