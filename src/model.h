#pragma once

#include <memory>
#include "shader.h"
#include "component.h"
#include "bounds.h"


class IBatch;

class Renderer;

class Model {
public:
    Model();

    virtual ~Model();

	//void setTexture(const std::string& texFile);

    virtual std::shared_ptr<Renderer> getRenderer(IBatch*) const = 0;

	Bounds getBounds() const;
protected:
    //ShaderType m_shaderType;
    //GLuint m_size;
	Bounds m_modelBounds;
};

inline Bounds Model::getBounds() const {
	return m_modelBounds;
}

