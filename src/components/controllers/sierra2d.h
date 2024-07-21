#pragma once

#include "../renderer.h"
#include "../../runners/collision_engine.h"
#include "../../runners/walkarea.h"

class Sierra2DController : public Component {
public:
    explicit Sierra2DController(const pybind11::kwargs& args);
    void setDirection(const std::string& direction);
    std::string getDirection() const;
	using Base = Sierra2DController;
	void start() override;
	//std::type_index getType() override;
protected:
    void update(double) override;
    void updateZ(float, float);

    std::string _direction;
	WalkArea* _walkArea;

private:
    pybind11::function _zFunc;
    pybind11::function _scaleFunc;
    //float _yFront;
    //float _yBack;
    //float _a, _b;
    glm::vec3 _previousPosition;
};

inline std::string Sierra2DController::getDirection() const {
    return _direction;
}

inline void Sierra2DController::setDirection(const std::string &direction) {
	_direction = direction;
}

class PlayerSierra2DController : public Sierra2DController {
public:
	explicit PlayerSierra2DController(const pybind11::kwargs& args);
	//void move(glm::vec3&, bool forced) override;

	void start() override;
	void update(double) override;

	void enable(bool);
	void setAnim(const std::string& idle, const std::string& walk);
	using Base = PlayerSierra2DController;

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
	float _speed;
};

class CustomSierra2DController : public Sierra2DController {
public:
    explicit CustomSierra2DController(const pybind11::kwargs& args);
    void update(double) override;
private:
    pybind11::function _callback;
};