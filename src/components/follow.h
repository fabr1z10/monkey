#pragma once

#include "../component.h"
#include "../camera.h"

class Follow : public Component {
public:
	Follow (std::shared_ptr<Camera> cam, pybind11::tuple& c, pybind11::tuple& a);
	void update(double) override;

protected:
	glm::vec3 m_relativePos;
	glm::vec3 m_up;
	std::shared_ptr<Camera> m_cam;
	glm::vec3 m_previous;
};

