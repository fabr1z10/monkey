#pragma once

#include <GL/glew.h>
#include <list>
#include "camera.h"
#include "node.h"

class Shader;

// every batch is associated to a camera. THis is important because
// batch uses this camera to setup the projection view matrix.

class IBatch {
public:
	IBatch(int verticesPerElement, const pybind11::kwargs&);
	void init();
	virtual void setupUniforms(Shader* s);
	virtual void draw(Shader* s) = 0;
	virtual void cleanUp() = 0;
	ShaderType getShaderType() const;
	virtual void configure(Shader* s) = 0;
//	Camera* getCamera() {
//		return _cam.get();
//	};
	// add current node to this batch for rendering
	//void add(Node*, const pybind11::kwargs& args = pybind11::kwargs());

    int getPrimitiveId() ;

    virtual void release(int id);

	virtual void setVisible(int index, bool) = 0;

	int getCameraId() const;
protected:
	int _maxElements; 			// max number of elements (e.g quads or lines)
	int _vertsPerElement;		// vertices per element
	int _camId;
	ShaderType _shaderType;
	Camera* _cam;

    int _nPrimitive;			// next element to be allocated
    std::list<int> _deallocated;
};

inline ShaderType IBatch::getShaderType() const {
	return _shaderType;
}

inline int IBatch::getCameraId() const {
	return _camId;
}


// implements batch rendering
template<typename VERTEXDATA>
class Batch : public IBatch{
public:
    Batch(int vertsPerElement, int indicesPerElement, const pybind11::kwargs& args) : IBatch(vertsPerElement, args),
    	_indicesPerElement(indicesPerElement)  {
    	_data.resize(_maxElements * vertsPerElement);
    }

    virtual ~Batch() {
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
    }

    void draw(Shader* s) {
    	if (s->getShaderType() != _shaderType) {
    		return;
    	}

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEXDATA) * _nPrimitive * _vertsPerElement, &_data[0]);

		//_cam->init(s);

		// set view matrix
		//s->setMat4("view", _cam->getViewMatrix());

		initDraw(s);
		glBindVertexArray(_vao);
		glDrawElements(_prim, _indicesPerElement * _nPrimitive, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
	}

    void cleanUp() {
		_nPrimitive = 0;
		_deallocated.clear();
		_data = std::vector<VERTEXDATA>(_maxElements);
	}

	void release(int index) override {
        IBatch::release(index);
        //_deallocated.push_back(index);
        int offset = index * _vertsPerElement;
        memset(&_data[offset], 0, sizeof(VERTEXDATA) * _vertsPerElement);
    }



    void configure(Shader* s) override {
		if (s->getShaderType() != _shaderType) {
			return;
		}
		auto totalVertices = _maxElements * _vertsPerElement;
    	_data.resize(totalVertices);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// create buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEXDATA) * _data.size(), &_data[0], GL_DYNAMIC_DRAW);

		innerConfigure();


	}
	virtual void initDraw(Shader* s) = 0;
	virtual void innerConfigure() = 0;
    //virtual void computeOffsets(GLuint) = 0;

    virtual int getPalette(const std::string&) { return 0; }


protected:
    // index of next primitive
    //int _bytesPerVertex;
    int _indicesPerElement;
    GLenum _prim;
    unsigned _verticesPerPrimitive;
    GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
    GLint _blockSize;


    std::vector<VERTEXDATA> _data;


};


class ProvaBatch : public IBatch {
public:
	ProvaBatch(const pybind11::kwargs& kwargs);
	void draw(Shader* s) override;
	void cleanUp() {}
	void configure(Shader* s) override {}
	void setVisible(int index, bool value) override {}
	void addRenderer(Renderer*);
	void removeRenderer(Renderer*);
private:
	std::unordered_map<int, Renderer*> _renderers;

};