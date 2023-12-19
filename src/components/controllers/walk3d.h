#include "../controller.h"
#include "../renderer.h"
#include "../../runners/collision_engine.h"



class Walk3DController : public Component {
public:
    explicit Walk3DController(float size, float speed, float gravity);
    //void move(glm::vec3&, bool forced) override;

    void start() override;
    void update(double) override;

    using Base = Walk3DController;

private:
    float _xCorrection;
    float _yVelocity;
    float _gravity;
    float _size;
    float _skinWidth;
    float _speed;
    bool _lookingLeft;
    Renderer* m_animatedRenderer;
    ICollisionEngine* m_collisionEngine;

};