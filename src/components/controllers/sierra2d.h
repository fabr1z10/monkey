#pragma once

#include "../renderer.h"
#include "../../runners/collision_engine.h"


class Sierra2DController : public Component {
public:
    explicit Sierra2DController(const pybind11::kwargs& args);
protected:
    void update(double) override;
    void updateZ(float, float);
private:
    pybind11::function _zFunc;
    pybind11::function _scaleFunc;
    //float _yFront;
    //float _yBack;
    //float _a, _b;
    glm::vec3 _previousPosition;

};


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