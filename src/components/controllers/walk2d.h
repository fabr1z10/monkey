//#pragma once
//
//#include "../../component.h"
//#include "../controller2d.h"
//#include "../../keylistener.h"
//
//
//class Walk2D : public Component {
//
//
//
//public:
//
//
//	Walk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//		const pybind11::kwargs& args);
//	void start() override;
//	void update(double) override;
//	virtual void control() = 0;
//
//    int getDirection() const;
//    void setDirection(int);
//
//    float getSpeed() const;
//    void setSpeed(float);
//
//    void jump();
//
//    bool grounded() const;
//protected:
//    float _dir;
//	Controller2D* _controller;
//	glm::vec3 _v;
//	glm::vec3 _a;
//	float _gravity;
//	float _jumpHeight;
//	float _timeToJumpApex;
//	float _jumpVelocity;           // calculated
//	float _acceleration;
//	float _maxSpeedGround;
//	//float _maxSpeedAir;
//	float _accelerationTime;
//	//int _flags;
//	int _direction;
//    //bool _jumping;
//	bool _active;
//    bool _flipH;
//	// bits
//	// 0 - face (0=right, 1=left)
//	// 1 - moving (0=false, 1=true)
//	// 2 - direction (0=fwd, 1=back)
//};
//
//inline bool Walk2D::grounded() const {
//	return _controller->grounded();
//}
//
//inline int Walk2D::getDirection() const {
//    return _direction;
//}
//
//inline void Walk2D::setDirection(int value) {
//    _direction = value;
//}
//
//inline float Walk2D::getSpeed() const {
//    return _maxSpeedGround;
//}
//
//inline void Walk2D::setSpeed(float value) {
//    _maxSpeedGround = value;
//}
//
//
//class PlayerWalk2D : public Walk2D, public KeyboardListener {
//public:
//	PlayerWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//			  const pybind11::kwargs& args);
//	void control() override;
//	int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
//private:
//	int _jmpKey;
//	int _btn1;
//	int _btn2;
//	int _btn3;
//};
//
//class CustomWalk2D : public Walk2D, public KeyboardListener {
//public:
//    CustomWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//                 const pybind11::object internal, const pybind11::kwargs& args);
//    void control() override;
//    int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
//    bool isPressed(int key);
//    void start() override;
//
//private:
//    pybind11::object _internal;
//    pybind11::function _internalControl;
//};
//
//class FoeWalk2D : public Walk2D {
//public:
//    FoeWalk2D(float maxSpeed, float accelerationTime, float jumpHeight, float timeToJumpApex, const pybind11::kwargs& args);
//    void control() override;
//
//private:
//    bool _flipPlatformEdge;
//
//    int _status;
//
//};
