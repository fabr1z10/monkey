#include "../runner.h"
#include "../light.h"
#include <vector>
#include <memory>

class Lighting : public Runner {
public:
	Lighting();
	void setupLights(Shader*);
	void setAmbient(float ambient);
	void addLight(std::shared_ptr<Light> light);
	void update(double) override {}
private:
	std::vector<std::shared_ptr<Light>> m_lights;
	float m_ambientStrength;

};
