#pragma once

#include "../component.h"
#include "../shader.h"
#include "../model.h"
#include "../models/drawablemodel.h"
#include <pybind11/pybind11.h>

class Renderer : public Component {
public:
    Renderer();
    //virtual int setup(Shader*);
    virtual void draw(Shader*) {}


    //std::shared_ptr<Model> getModel();
    virtual void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) {}
    //void setMultColor(glm::vec4);
    //void setAddColor(glm::vec4);
    const glm::mat4& getRendererTransform() const;
    void flipHorizontal(bool);
    bool getFlipHorizontal() const;
    ShaderType getShaderType() const;
    void setTransform(const glm::mat4& m);
    void setCount(int);
    void setOffset(int);
    virtual void setPalette(const std::string& palId) = 0;
    //virtual void setPalette(int quad, int pal) {}
	using Base = Renderer;
	std::string getAnimation() const;
    virtual void setAnimation(const std::string&) {}
    //virtual bool isComplete() const;
    void setShift(glm::vec3);
    void setZLayer(float);
protected:
	ShaderType _shaderType;
	std::string m_animation;

    //std::shared_ptr<Model> m_model;
    //glm::vec4 m_multColor;
    //glm::vec4 m_addColor;
    glm::mat4 m_rendererTransform;
    int m_offset;
    int m_count;
    int _paletteId;
	float _zLayer;
    glm::vec3 m_shift;
	//GLuint m_texId;
	//GLuint m_paletteId;
};


inline ShaderType Renderer::getShaderType() const {
	return _shaderType;
}
//inline std::shared_ptr<Model> Renderer::getModel() {
//	return m_model;
//}

inline void Renderer::setZLayer(float z) {
	_zLayer = z;
}

inline std::string Renderer::getAnimation() const {
	return m_animation;
}

class BasicRenderer : public Renderer {
public:
	BasicRenderer(ShaderType type);
	void draw(Shader*) override;
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;

private:

	std::shared_ptr<DrawableModel> m_model;
};