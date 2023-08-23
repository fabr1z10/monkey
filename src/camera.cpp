#include "camera.h"
#include "engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <pybind11/stl.h>
#include <cassert>
#include "pyhelper.h"

// ** every ** camera needs to have a viewport. A viewport represents
// a portion of the device size where the camera projects its image.
// viewport is 4 numbers: first two is the bottom left position of the rectangle,
// and the last 2 are the size (width and height)
Camera::Camera(const py::kwargs& kwargs) {
	//_id = py_get_dict<std::string>(kwargs, "name");
    auto deviceSize = Engine::instance().getDeviceSize();

    m_viewport = py_get_dict<glm::vec4>(kwargs, "viewport", glm::vec4(0, 0, deviceSize[0], deviceSize[1]));

    auto t = std::numeric_limits<float>::infinity();

    m_xBounds = py_get_dict<glm::vec2>(kwargs, "bounds_x", glm::vec2(-t, t));

    m_yBounds = py_get_dict<glm::vec2>(kwargs, "bounds_y", glm::vec2(-t, t));
    m_zBounds = py_get_dict<glm::vec2>(kwargs, "bounds_z", glm::vec2(-t, t));
    setPosition(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


}

glm::vec4 Camera::getViewport() const {
	return m_viewport;
}

OrthoCamera::OrthoCamera(float width, float height, const py::kwargs& kwargs) : Camera(kwargs), m_orthoWidth(width), m_orthoHeight(height) {
    float hw = m_orthoWidth / 2.0f;
    float hh = m_orthoHeight / 2.0f;
    m_projectionMatrix = glm::ortho(-hw, hw, -hh, hh, -100.0f, 100.0f);
	m_halfSize = glm::vec3(hw, hh, 100.f);
}

glm::vec2 OrthoCamera::getWorldCooridnates(float x, float y) {
    float xw = -m_viewMatrix[3][0] - (m_orthoWidth * 0.5f) + (x - m_viewport.x) * (m_orthoWidth / m_viewport[2]);
    float yw = -m_viewMatrix[3][1] - (m_orthoHeight * 0.5f) + (y - m_viewport.y) * (m_orthoHeight / m_viewport[3]);
    return glm::vec2(xw, yw);
}

PerspectiveCamera::PerspectiveCamera(const py::kwargs& kwargs) : Camera(kwargs) {
    m_fov = glm::radians(py_get_dict<float>(kwargs, "fov", 45.0f));
    m_near = py_get_dict<float>(kwargs, "near", 0.05f);
    m_far = py_get_dict(kwargs, "far", 100.0f);
    m_projectionMatrix = glm::perspective (m_fov, m_viewport[2]/m_viewport[3], m_near, m_far);

}

void Camera::move(glm::vec2 delta) {
    m_eye += glm::vec3(delta, 0.f);
    m_eye.x = std::clamp(m_eye.x, m_xBounds[0], m_xBounds[1]);
    m_eye.y = std::clamp(m_eye.y, m_yBounds[0], m_yBounds[1]);
    m_eye.z = std::clamp(m_eye.z, m_zBounds[0], m_zBounds[1]);
    m_viewMatrix = glm::lookAt(m_eye, m_eye + m_fwd, m_up);

}

void Camera::setPosition(glm::vec3 eye, glm::vec3 dir, glm::vec3 up) {
    m_fwd = dir;
    m_up = up;
    m_eye = eye;
    m_eye.x = std::clamp(m_eye.x, m_xBounds[0], m_xBounds[1]);
    m_eye.y = std::clamp(m_eye.y, m_yBounds[0], m_yBounds[1]);
    m_eye.z = std::clamp(m_eye.z, m_zBounds[0], m_zBounds[1]);
    m_viewMatrix = glm::lookAt(m_eye, m_eye + dir, up);
}

void Camera::init(Shader* s) {
    // setup projection and viewport
    s->setMat4("projection", m_projectionMatrix);
    //glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &m_projectionMatrix[0][0]);
    auto vp = Engine::instance().getActualDeviceViewport();
    //glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);

    // the actual viewport x of the current camera will be the x of the window viewport + local x scaled by the viewport
    // scaling factor
    float x = vp.x + m_viewport.x * vp[2];
    float y = vp.y + m_viewport.y * vp[3];
    float w = m_viewport[2] * vp[2];
    float h = m_viewport[3] * vp[3];
    m_screenViewport = glm::vec4(x, y, x+w, y+h);
    glViewport(vp.x, vp.y, vp.z, vp.w);


}

void Camera::setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
    m_xBounds = glm::vec2(xMin, xMax);
    m_yBounds = glm::vec2(yMin, yMax);
    m_zBounds = glm::vec2(zMin, zMax);
    if (m_eye.x < xMin || m_eye.x > xMax) m_eye.x = xMin;
    if (m_eye.y < yMin || m_eye.y > yMax) m_eye.x = yMin;
    setPosition(m_eye, m_fwd, m_up);

}

bool Camera::isInViewport(float x, float y) {
    float winHeight = Engine::instance().getWindowSize().y;
    float yf = winHeight - y;
    if (x < m_screenViewport[0] || x > m_screenViewport[2]) {
        return false;
    }
    if (yf < m_screenViewport[1] || yf > m_screenViewport[3]) {
        return false;
    }
    return true;

}

Bounds OrthoCamera::getViewingBounds() const {
	Bounds b;
	b.min = m_eye - m_halfSize;
	b.max = m_eye + m_halfSize;
	return b;
}

Bounds PerspectiveCamera::getViewingBounds() const {
	return Bounds();
}