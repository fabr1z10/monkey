#pragma once

#include "../component.h"
#include "../shader.h"
#include "../model.h"

class Renderer : public Component {
public:
    Renderer();
    virtual int setup(Shader*);
    virtual void draw(Shader*);
    std::shared_ptr<Model> getModel();
    virtual void setModel(std::shared_ptr<Model>);
    void setMultColor(glm::vec4);
    void setAddColor(glm::vec4);
    const glm::mat4& getRendererTransform() const;
    void flipHorizontal(bool);
    bool getFlipHorizontal() const;
    void setTransform(const glm::mat4& m);
    void setCount(int);
    void setOffset(int);
    ShaderType getShaderType() const;
protected:
    std::shared_ptr<Model> m_model;
    glm::vec4 m_multColor;
    glm::vec4 m_addColor;
    glm::mat4 m_rendererTransform;
    int m_offset;
    int m_count;
};

