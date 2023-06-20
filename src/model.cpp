#include "model.h"
#include "components/renderer.h"
#include "assetmanager.h"

Model::Model() /*m_vbo(GL_INVALID_VALUE), m_ebo(GL_INVALID_VALUE),m_texId(GL_INVALID_VALUE), m_paletteId(GL_INVALID_VALUE) */{}

//Model::Model(int shaderType, GLuint primitive) : m_shaderType(static_cast<ShaderType>(shaderType)), m_primitive(primitive),
//    /*m_vbo(GL_INVALID_VALUE), m_ebo(GL_INVALID_VALUE),*/ m_texId(GL_INVALID_VALUE), m_paletteId(GL_INVALID_VALUE)
//{}

Model::~Model() {
}

//
//std::shared_ptr<Renderer> Model::getRenderer() const {
//    return std::make_shared<Renderer>();
//}



//void Model::setTexture(const std::string &texFile) {
//	auto& am = AssetManager::instance();
//	auto tex = am.getTex(texFile);
//	//m_texId = tex->getTexId();
//
//}