#pragma once



#include "controller.h"



// this is for 2D platformers
class MarioController : public Controller {
public:
	explicit MarioController(const pybind11::kwargs&);
	void move(glm::vec3&, bool forced) override;
	void updateRaycastOrigins() override {}
	bool isFalling(float) { return false;}
	std::shared_ptr<Model> getDebugModel() override;
	std::type_index getType() override;
private:
	glm::vec3 _position;
	float m_skinWidth;
	float m_maxClimbAngle;
	bool _slope;
	glm::vec2 _slopeNormal;
};