#include "../controller.h"
#include "../renderer.h"
#include "../../runners/collision_engine.h"
#include "../../keylistener.h"

class Walk3DController;

class Walk3DListener : public KeyboardListener {
public:
    Walk3DListener(Walk3DController* controller) : KeyboardListener(), _controller(controller) {}
    int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
private:
    Walk3DController* _controller;
};

class Walk3DController : public Component {
public:
    explicit Walk3DController(float size, float speed, float gravity, const pybind11::kwargs& args);
    //void move(glm::vec3&, bool forced) override;

    void start() override;
    void update(double) override;

    using Base = Walk3DController;

    void playAnimation(const std::string&);

    void jump();
private:
    std::unique_ptr<Walk3DListener> _listener;
    float _xCorrection;
    float _yVelocity;
    float _gravity;
    float _size;
    float _skinWidth;
    float _speed;
    bool _lookingLeft;
    bool _grounded;
    Renderer* m_animatedRenderer;
    ICollisionEngine* m_collisionEngine;
    std::string _walkAnim;
    std::string _idleAnim;
    bool _playingAnimation;
};