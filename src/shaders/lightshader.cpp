#include "lightshader.h"
#include "../engine.h"
#include "../runners/lighting.h"

void LightShader::use() {
	Shader::use();
	auto* lighting = Engine::instance().getRoom()->getRunner<Lighting>();
	lighting->setupLights(this);
}


void LightShader::init(Node* node) {
	const auto& m = node->getWorldMatrix();
	auto normalMat = glm::mat3(glm::transpose(glm::inverse(m)));
	setMat3("nMat", normalMat);
}
