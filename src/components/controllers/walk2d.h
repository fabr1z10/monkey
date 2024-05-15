#pragma once

#include "../../component.h"
#include "../controller2d.h"
#include "../../keylistener.h"


class Walk2D : public Component {



public:


	Walk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
		const pybind11::kwargs& args);
	void start() override;
	void update(double) override;
	virtual void control() = 0;


protected:
    float _dir;
	Controller2D* _controller;
	glm::vec3 _v;
	glm::vec3 _a;
	float _gravity;
	float _jumpHeight;
	float _timeToJumpApex;
	float _jumpVelocity;           // calculated
	float _acceleration;
	float _maxSpeedGround;
	float _maxSpeedAir;
	float _accelerationTime;
	//int _flags;
	int _direction;
    //bool _jumping;
	bool _active;
	// bits
	// 0 - face (0=right, 1=left)
	// 1 - moving (0=false, 1=true)
	// 2 - direction (0=fwd, 1=back)
};

class PlayerWalk2D : public Walk2D, public KeyboardListener {
public:
	PlayerWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
			  const pybind11::kwargs& args);
	void control() override;
	int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

private:
	int _jmpKey;
	int _btn1;
	int _btn2;
	int _btn3;
};

class FoeWalk2D : public Walk2D {
public:
    FoeWalk2D(float maxSpeed, float accelerationTime, float jumpHeight, float timeToJumpApex, const pybind11::kwargs& args);
    void control() override;

private:
    bool _flipPlatformEdge;
    bool _flipH;
    int _status;

};
