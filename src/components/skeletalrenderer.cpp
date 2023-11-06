#include "skeletalrenderer.h"
#include <glm/gtc/type_ptr.hpp>

SkeletalRenderer::SkeletalRenderer(const pybind11::kwargs& args) : BatchRenderer<ProvaBatch>(args) {
//	_shaderType = SHADER_SKELETAL;
}

void SkeletalRenderer::draw(Shader * s) {

	auto boneLoc = glGetUniformLocation(s->getProgId(), "Bone");
	auto l2m = glGetUniformLocation(s->getProgId(), "local_to_model");
	auto weightIndex = glGetUniformLocation(s->getProgId(), "weightIndex");
	auto pz = glGetUniformLocation(s->getProgId(), "z");
	glUniformMatrix4fv(boneLoc, _bones.size(), false, &_bones[0][0][0]);
	int n{0};
	auto worldMatrix = m_node->getWorldMatrix();
	int jointMatrixLoc = glGetUniformLocation(s->getProgId(), "model");
		glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(worldMatrix[0]));
	for (const auto& model : _model->getModels()) {
		auto restTransform = _model->getRestTransform(n);
		const auto& jinfo = _model->getJointInfo(n++);
		auto weightIndices = jinfo.weightIndex;// m_skeletalModel->getWeightIndex(n++);
		glUniformMatrix4fv(l2m, 1, false, &restTransform[0][0]);
		glUniform3iv(weightIndex, 1, &weightIndices[0]);
		glUniform1f(pz, jinfo.z);
		model->draw(s);
	}

}

void SkeletalRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	_model = dynamic_cast<SkeletalModel*>(model.get());
	assert(_model != nullptr);
	_batch->addRenderer(this);
}

SkeletalRenderer::~SkeletalRenderer() noexcept {
	_batch->removeRenderer(this);
}