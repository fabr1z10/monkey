#include "walk3d.h"

class PlayerWalk3D : public Walk3D {
public:
	PlayerWalk3D(const pybind11::kwargs& kwargs);
	void control() override;
	void keyCallback(int key) override;
private:
	int _keyJump;
};

