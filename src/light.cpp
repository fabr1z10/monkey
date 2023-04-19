#include "light.h"
#include "pyhelper.h"


DirectionalLight::DirectionalLight(const pybind11::kwargs& args) : Light() {
	m_direction = py_get_dict<glm::vec3>(args, "direction");
	m_diffuseColor = py_get_dict<glm::vec3>(args, "diffuse");
}




void DirectionalLight::setup(Shader* s) {
	s->setVec3("lightDir", m_direction);
	s->setVec3("diffuse", m_diffuseColor);


}