#pragma once

#include <glm/glm.hpp>
#include <array>
#include <pybind11/pybind11.h>
#include "shader.h"
#include "bounds.h"

namespace py = pybind11;

class Camera {
public:
    explicit Camera(const py::kwargs& kwargs);
    ~Camera() =default;
    void setPosition(glm::vec3 eye, glm::vec3 direction, glm::vec3 up = glm::vec3(0, 1, 0));
    void move(glm::vec2);
    void setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
    void init(Shader*);
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;
    bool isInViewport(float, float);
    virtual Bounds getViewingBounds() const = 0;
    //std::string getCameraName() const;
    glm::vec4 getViewport() const;
protected:
    glm::vec3 _fwd;
    glm::vec3 _up;
    glm::vec3 _eye;
    glm::vec2 _xBounds;
    glm::vec2 _yBounds;
    glm::vec2 _zBounds;

    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    glm::vec4 _viewport;
    glm::vec4 _screenViewport;
    //std::string _id;
};

inline const glm::mat4 & Camera::getViewMatrix() const {
    return _viewMatrix;
}

inline const glm::mat4 & Camera::getProjectionMatrix() const {
	return _projectionMatrix;
}


class OrthoCamera : public Camera {
public:
    OrthoCamera(float width, float height, const py::kwargs& kwargs);
    glm::vec2 getWorldCooridnates(float x, float y);
	Bounds getViewingBounds() const override;
private:
    float _orthoWidth;
    float _orthoHeight;
    glm::vec3 _halfSize;

};


//class PerspectiveCamera : public Camera {
//public:
//    PerspectiveCamera(const py::kwargs& kwargs);
//	Bounds getViewingBounds() const override;
//private:
//    float m_fov;
//    float m_near;
//    float m_far;
//    float m_aspectRatio;
//};