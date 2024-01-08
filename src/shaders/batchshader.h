#pragma ONCE

#include "../shader.h"


class BatchShader : public Shader {
public:
    BatchShader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode, const std::string& vertexFormat);
    void draw() override;
};