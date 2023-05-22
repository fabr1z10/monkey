#include "walk2d.h"


class PlayerWalk2D : public Walk2D {
public:
	PlayerWalk2D() : Walk2D() {}
	void control() override;

	void keyCallback(int key) override;
private:
	bool m_wasGroudned;
};


