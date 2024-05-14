#include <glm/gtc/type_ptr.hpp>
#include "renderer.h"
#include "../node.h"
#include "../assetmanager.h"

Renderer::Renderer() : Component(), m_rendererTransform(1.f),
    m_offset(0), m_count(0), m_shift(glm::vec3(0.f)), _zLayer(0.f) {

}

const glm::mat4 & Renderer::getRendererTransform() const {
    return m_rendererTransform;
}





//void Renderer::draw(Shader * s) {
//    m_model->draw(s, m_offset, m_count);
//}

//void Renderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
//    m_model = model;
//}
//
//void Renderer::setMultColor(glm::vec4 multColor) {
//    m_multColor = multColor;
//}
//
//
//
//void Renderer::setAddColor(glm::vec4 addColor) {
//    m_addColor = addColor;
//}

void Renderer::flipHorizontal(bool value) {
    m_rendererTransform[0] = abs(m_rendererTransform[0]) * (value ? -1.f : 1.f);
}

bool Renderer::getFlipHorizontal() const {
    return m_rendererTransform[0][0] < 0.f;
}

void Renderer::setCount(int count) {
    m_count = count;
}

void Renderer::setOffset(int offset) {
    m_offset = offset;
}


void Renderer::setTransform(const glm::mat4 &m) {
    m_rendererTransform = m;
}

//bool Renderer::isComplete() const {
//    return false;
//}

void Renderer::setShift(glm::vec3 shift) {
	m_shift = shift;
}

BasicRenderer::BasicRenderer(ShaderType type) : Renderer() {
	_shaderType = type;
}

void BasicRenderer::draw(Shader * s) {
	auto worldMatrix = m_node->getWorldMatrix();
	int jointMatrixLoc = glGetUniformLocation(s->getProgId(), "model");
	glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(worldMatrix[0]));
	m_model->draw(s);
}
void BasicRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs &args) {
	m_model = std::dynamic_pointer_cast<DrawableModel>(model);
	assert(m_model != nullptr);
}