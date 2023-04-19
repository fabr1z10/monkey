#include "lighting.h"

Lighting::Lighting() : Runner() {}

void Lighting::setupLights(Shader* s) {
	s->setFloat("ambient", m_ambientStrength);
	for (const auto& light : m_lights) {
		light->setup(s);
	}

}

void Lighting::addLight(std::shared_ptr<Light> light) {
	m_lights.push_back(light);
}


void Lighting::setAmbient(float ambient) {
	m_ambientStrength = ambient;
}