#pragma once

#include "../component.h"
#include "../shader.h"
#include "../model.h"
#include <pybind11/pybind11.h>

class Renderer : public Component {
public:
    Renderer();
    virtual int setup(Shader*);
    //virtual void draw(Shader*);


    std::shared_ptr<Model> getModel();
    virtual void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args = pybind11::kwargs());
    void setMultColor(glm::vec4);
    void setAddColor(glm::vec4);
    const glm::mat4& getRendererTransform() const;
    void flipHorizontal(bool);
    bool getFlipHorizontal() const;
    void setTransform(const glm::mat4& m);
    void setCount(int);
    void setOffset(int);
    ShaderType getShaderType() const;
    void setPalette(unsigned paletteId);
	using Base = Renderer;
    virtual void setAnimation(const std::string&) {}
    virtual bool isComplete();
    void setShift(glm::vec3);
    void setZLayer(float);
protected:

    std::shared_ptr<Model> m_model;
    glm::vec4 m_multColor;
    glm::vec4 m_addColor;
    glm::mat4 m_rendererTransform;
    int m_offset;
    int m_count;
    unsigned _paletteId;
	float _zLayer;
    glm::vec3 m_shift;
	//GLuint m_texId;
	//GLuint m_paletteId;
};

inline std::shared_ptr<Model> Renderer::getModel() {
	return m_model;
}

inline void Renderer::setZLayer(float z) {
	_zLayer = z;
}