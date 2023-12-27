#include "cam25.h"
#include "pyhelper.h"

#include <glm/gtc/matrix_transform.hpp>

Camera25::Camera25(float width, float height, const pybind11::kwargs& args) : OrthoCamera(width, height, args) {
	m_angle = glm::radians(py_get_dict<float>(args, "angle", 45.0f));
	//m_cosAngle = cos(m_angle);
	//m_sinAngle = sin(m_angle);
    float hw = width / 2.0f;
    float hh = height / 2.0f;
    m_projectionMatrix = glm::ortho(-hw, hw, -hh, hh, -1000.0f, 1000.0f);

    m_projectionMatrix[2][0] = -0.008f;
    m_projectionMatrix[2][1] = -0.008f;
}

//void Camera25::Init() {
//	SetPosition(m_eye, m_fwd, m_up, true);
//
//}

void Camera25::setPosition(glm::vec3 eye, glm::vec3 dir, glm::vec3 up) {
	m_eye.x = std::clamp(eye.x, m_xBounds[0], m_xBounds[1]);
	m_eye.y = std::clamp(eye.y, m_yBounds[0], m_yBounds[1]);
	m_eye.z = std::clamp(eye.z, m_zBounds[0], m_zBounds[1]);

	//if (!isEqual(eye.x, m_eye.x) || !isEqual(eye.y, m_eye.y) || !isEqual(eye.z, m_eye.z)) {
	m_fwd = dir;
	m_up = up;

    //    glm::vec3 caz = glm::normalize(glm::vec3(-m_sinAngle, -m_sinAngle, 1.f));

	m_viewMatrix = glm::mat4(1.0f);
	//m_viewMatrix[1][1] = 1.0f;
	//m_viewMatrix[2][0] = caz[0];
	//m_viewMatrix[2][1] = caz[1];
	m_viewMatrix[3][0] = -m_eye.x;
	m_viewMatrix[3][1] = -m_eye.y;
	m_viewMatrix[3][2] = -m_eye.z;
	//m_viewMatrix[2][2] = caz[2];
		//OnMove.Fire(this);
	//}

}