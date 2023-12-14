#include "../controller.h"
#include "../renderer.h"
#include "../../runners/collision_engine.h"



class Sierra2DController : public Component {
public:
	explicit Sierra2DController(const pybind11::kwargs& args);
	//void move(glm::vec3&, bool forced) override;

	void start() override;
	void update(double) override;

	void enable(bool);
	void setAnim(const std::string& idle, const std::string& walk);
	using Base = Sierra2DController;

private:
	bool _enableControls;
	Renderer* m_animatedRenderer;
	ICollisionEngine* m_collisionEngine;

	std::string _dir;
	bool _lookingLeft;
	float _halfWidth;
	float _skinWidth;
	std::string _idleAnimation;
	std::string _walkAnimation;
	float _yFront;
	float _yBack;
	float _a, _b;
};