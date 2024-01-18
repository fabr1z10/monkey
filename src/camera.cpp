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

    _viewport = py_get_dict<glm::vec4>(kwargs, "viewport", glm::vec4(0, 0, deviceSize[0], deviceSize[1]));

    auto t = std::numeric_limits<float>::infinity();

    _xBounds = py_get_dict<glm::vec2>(kwargs, "bounds_x", glm::vec2(-t, t));
    _yBounds = py_get_dict<glm::vec2>(kwargs, "bounds_y", glm::vec2(-t, t));
    _zBounds = py_get_dict<glm::vec2>(kwargs, "bounds_z", glm::vec2(-t, t));
    setPosition(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


}

glm::vec4 Camera::getViewport() const {
	return _viewport;
}

OrthoCamera::OrthoCamera(float width, float height, const py::kwargs& kwargs) : Camera(kwargs), _orthoWidth(width), _orthoHeight(height) {
    float hw = _orthoWidth / 2.0f;
    float hh = _orthoHeight / 2.0f;
    _projectionMatrix = glm::ortho(-hw, hw, -hh, hh, -100.0f, 100.0f);
	_halfSize = glm::vec3(hw, hh, 100.f);
}

glm::vec2 OrthoCamera::getWorldCooridnates(float x, float y) {
    float xw = -_viewMatrix[3][0] - (_orthoWidth * 0.5f) + (x - _viewport.x) * (_orthoWidth / _viewport[2]);
    float yw = -_viewMatrix[3][1] - (_orthoHeight * 0.5f) + (y - _viewport.y) * (_orthoHeight / _viewport[3]);
    return glm::vec2(xw, yw);
}

//PerspectiveCamera::PerspectiveCamera(const py::kwargs& kwargs) : Camera(kwargs) {
//    m_fov = glm::radians(py_get_dict<float>(kwargs, "fov", 45.0f));
//    m_near = py_get_dict<float>(kwargs, "near", 0.05f);
//    m_far = py_get_dict(kwargs, "far", 100.0f);
//    m_projectionMatrix = glm::perspective (m_fov, m_viewport[2]/m_viewport[3], m_near, m_far);
//
//}

void Camera::move(glm::vec2 delta) {
    _eye += glm::vec3(delta, 0.f);
    _eye.x = std::clamp(_eye.x, _xBounds[0], _xBounds[1]);
    _eye.y = std::clamp(_eye.y, _yBounds[0], _yBounds[1]);
    _eye.z = std::clamp(_eye.z, _zBounds[0], _zBounds[1]);
    _viewMatrix = glm::lookAt(_eye, _eye + _fwd, _up);

}

void Camera::setPosition(glm::vec3 eye, glm::vec3 dir, glm::vec3 up) {
    _fwd = dir;
    _up = up;
    _eye = eye;
    _eye.x = std::clamp(_eye.x, _xBounds[0], _xBounds[1]);
    _eye.y = std::clamp(_eye.y, _yBounds[0], _yBounds[1]);
    _eye.z = std::clamp(_eye.z, _zBounds[0], _zBounds[1]);
    _viewMatrix = glm::lookAt(_eye, _eye + dir, up);
}

void Camera::init(Shader* s) {
    // setup projection and viewport
    s->setMat4("projection", _projectionMatrix);
    //glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &m_projectionMatrix[0][0]);
    auto vp = Engine::instance().getActualDeviceViewport();
    //glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);

    // the actual viewport x of the current camera will be the x of the window viewport + local x scaled by the viewport
    // scaling factor
    float x = vp.x + _viewport.x * vp[2];
    float y = vp.y + _viewport.y * vp[3];
    float w = _viewport[2] * vp[2];
    float h = _viewport[3] * vp[3];
    _screenViewport = glm::vec4(x, y, x+w, y+h);
    glViewport(vp.x, vp.y, vp.z, vp.w);


}

void Camera::setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
    _xBounds = glm::vec2(xMin, xMax);
    _yBounds = glm::vec2(yMin, yMax);
    _zBounds = glm::vec2(zMin, zMax);
    if (_eye.x < xMin || _eye.x > xMax) _eye.x = xMin;
    if (_eye.y < yMin || _eye.y > yMax) _eye.x = yMin;
    setPosition(_eye, _fwd, _up);

}

bool Camera::isInViewport(float x, float y) {
    float winHeight = Engine::instance().getWindowSize().y;
    float yf = winHeight - y;
    if (x < _screenViewport[0] || x > _screenViewport[2]) {
        return false;
    }
    if (yf < _screenViewport[1] || yf > _screenViewport[3]) {
        return false;
    }
    return true;

}

Bounds OrthoCamera::getViewingBounds() const {
	Bounds b;
	b.min = _eye - _halfSize;
	b.max = _eye + _halfSize;
	return b;
}

//Bounds PerspectiveCamera::getViewingBounds() const {
//	return Bounds();
//}