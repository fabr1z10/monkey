#include "../component.h"

class VerletObject : public Component {
public:


private:

    glm::vec2 _positionCurrent;
    glm::vec2 _positionOld;
    glm::vec2 _acceleration;
};