#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "symbols.h"

class Node;

struct VertexInfo {
    int size;
    GLenum type;
    unsigned long byteSize;
};


class Shader {
public:
    Shader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode, const std::string& vertexFormat);
    ~Shader();
    virtual void use();
    void init();
    void setupVertices();

    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    [[nodiscard]] ShaderType getShaderType() const;
    [[nodiscard]] GLuint getProgId() const;
    virtual void init(Node*) { }

    virtual void draw() {}
protected:
    GLuint m_programId;
    //GLuint m_vao;
    ShaderType m_shaderType;
    std::vector<VertexInfo> m_vertexFormat;
    GLsizei m_stride;
    static std::unordered_map<char, std::pair<GLenum, size_t>> m_types;
    unsigned m_flags;
};

inline GLuint Shader::getProgId() const {
    return m_programId;
}

inline ShaderType Shader::getShaderType() const {
    return m_shaderType;
}

//class SimpleShader : public Shader {
//public:
//	SimpleShader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode, const std::string& vertexFormat);
//	void draw() override;
//};

