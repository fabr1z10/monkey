    #include "renderer.h"
#include "../node.h"
#include "../assetmanager.h"

Renderer::Renderer() : Component(), m_multColor(glm::vec4(1.0f)), m_addColor(0.0f), m_rendererTransform(1.f),
    m_offset(0), m_count(0), m_shift(glm::vec3(0.f)) {

}

ShaderType Renderer::getShaderType() const {
    if (m_model == nullptr) {
        return ShaderType::NONE;
    }
    //return m_model->getShaderType();
}



int Renderer::setup(Shader * s) {
//    const auto& m = m_node->getWorldMatrix() * m_rendererTransform;
//    s->setVec4("mult_color", m_multColor);
//    s->setVec4("add_color", m_addColor);
//    s->setMat4("model", m);
//
//	if (m_paletteId != GL_INVALID_VALUE) {
//		s->setInt("texture_palette", 1);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_1D, m_paletteId);
//		if (m_texId != GL_INVALID_VALUE) {
//			s->setInt("texture_pdiffuse1", 0);
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//		}
//	} else {
//		if (m_texId != GL_INVALID_VALUE) {
//			s->setInt("texture_diffuse1", 0);
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//		}
//	}
//
//    return 0;
}

void Renderer::setPalette(unsigned palId) {
    _paletteId = palId;
//    auto& am = AssetManager::instance();
//    auto pal = am.getPalette(id);
//    m_paletteId = pal->getTexId();
}



//void Renderer::draw(Shader * s) {
//    m_model->draw(s, m_offset, m_count);
//}

void Renderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
    m_model = model;
}

void Renderer::setMultColor(glm::vec4 multColor) {
    m_multColor = multColor;
}

void Renderer::setAddColor(glm::vec4 addColor) {
    m_addColor = addColor;
}

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

bool Renderer::isComplete() {
    return false;
}

void Renderer::setShift(glm::vec3 shift) {
	m_shift = shift;
}