#include "../controller.h"
#include "../renderer.h"
#include "../../runners/collision_engine.h"



class Sierra2DController : public Component {
public:
	explicit Sierra2DController(const pybind11::kwargs& args);
	//void move(glm::vec3&, bool forced) override;

	void start() override;
	void update(double) override;
private:
	Renderer* m_animatedRenderer;
	ICollisionEngine* m_collisionEngine;

	std::string _dir;
	bool _lookingLeft;
	float _halfWidth;
	float _skinWidth;

};