#include "batchshader.h"
#include "../engine.h"


BatchShader::BatchShader(ShaderType type, const std::string &vertexCode, const std::string &fragmentCode,
    const std::string &vertexFormat) : Shader(type, vertexCode, fragmentCode, vertexFormat) {
}

void BatchShader::draw() {
    auto room = Engine::instance().getRoom();
    const auto& b = room->getBatches();
    for (const auto& batch : b) {
        if (m_shaderType == batch->getShaderType()) {
            batch->setupUniforms(this);
            batch->draw(this);
        }
    }
}
