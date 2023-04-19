#include "../shader.h"

class LightShader : public Shader {
public:
	LightShader(ShaderType type, const std::string& vertexCode, const std::string& fragmentCode, const std::string& vertexFormat)
		: Shader(type, vertexCode, fragmentCode, vertexFormat) {}

	void use() override ;
	void init(Node*) override;

};