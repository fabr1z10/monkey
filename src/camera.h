#pragma once

#include <glm/glm.hpp>
#include <array>
#include <pybind11/pybind11.h>
#include "shader.h"
#include "bounds.h"

namespace py = pybind11;

class Camera {
public:
    Camera(const py::kwargs& kwargs);
    ~Camera() {

    }
    virtual void setPosition(glm::vec3 eye, glm::vec3 direction, glm::vec3 up = glm::vec3(0, 1, 0));
    void move(glm::vec2);
    void setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
    void init(Shader*);
    const glm::mat4& getViewMatrix() const;
    bool isInViewport(float, float);
    virtual Bounds getViewingBounds() const = 0;
protected:
    glm::vec3 m_fwd;
    glm::vec3 m_up;
    glm::vec3 m_eye;
    glm::vec2 m_xBounds;
    glm::vec2 m_yBounds;
    glm::vec2 m_zBounds;

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::vec4 m_viewport;
    glm::vec4 m_screenViewport;
};

inline const glm::mat4 & Camera::getViewMatrix() const {
    return m_viewMatrix;
}


class OrthoCamera : public Camera {
public:
    OrthoCamera(float width, float height, const py::kwargs& kwargs);
    glm::vec2 getWorldCooridnates(float x, float y);
	Bounds getViewingBounds() const override;
private:
    float m_orthoWidth;
    float m_orthoHeight;
    glm::vec3 m_halfSize;

};


class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const py::kwargs& kwargs);
	Bounds getViewingBounds() const override;
private:
    float m_fov;
    float m_near;
    float m_far;
    float m_aspectRatio;
};