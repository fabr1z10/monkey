#include "walk2d.h"

// player can jump only if grounded

class PlayerWalk2D : public Walk2D {
public:
	PlayerWalk2D(const pybind11::kwargs& kwargs);
	void control() override;

	void keyCallback(int key) override;
private:
	bool m_wasGroudned;
	int _keyJump;
};


// class movexy (no gravity, no jump)
// class movexz (grvity, jump along y dir)

