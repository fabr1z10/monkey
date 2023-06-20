#pragma once

#include <GL/glew.h>
#include <list>
#include "camera.h"
#include "node.h"

class Shader;

class IBatch {
public:
	IBatch(int maxElements, int verticesPerElement) : _maxElements(maxElements), _vertsPerElement(verticesPerElement) {}
	virtual void draw(Shader* s) = 0;
	virtual void cleanUp() = 0;
	//ShaderType getShaderType() const;
	virtual void configure(Shader* s) = 0;
	Camera* getCamera() {
		return _cam.get();
	};
	// add current node to this batch for rendering
	void add(Node*, const pybind11::kwargs& args = pybind11::kwargs());
protected:
	int _maxElements; 			// max number of elements (e.g quads or lines)
	int _vertsPerElement;		// vertices per element

	//ShaderType _shaderType;
	std::shared_ptr<Camera> _cam;

};

//inline ShaderType IBatch::getShaderType() const {
//	return _shaderType;
//}


// implements batch rendering
template<typename VERTEXDATA>
class Batch : public IBatch{
public:
    Batch(int maxElements, int vertsPerElement, int indicesPerElement) : IBatch(maxElements, vertsPerElement),
    	_indicesPerElement(indicesPerElement), _nPrimitive(0) {
    	_data.resize(maxElements * vertsPerElement);
    }

    virtual ~Batch() {
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
    }

    void draw(Shader* s) {

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEXDATA) * _nPrimitive * _vertsPerElement, &_data[0]);

		_cam->init(s);
		// set view matrix
		s->setMat4("view", _cam->getViewMatrix());

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

    int getPrimitiveId() {
		int next;
		if (_deallocated.empty()) {
			next = _nPrimitive;
			_nPrimitive++;
		} else {
			next = _deallocated.front();
			_deallocated.pop_front();
		}
		return next;
	}

    void configure(Shader* s) override {

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



protected:
    // index of next primitive
    //int _bytesPerVertex;
    int _indicesPerElement;
	int _nPrimitive;			// next element to be allocated
    GLenum _prim;
    unsigned _verticesPerPrimitive;
    GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
    GLint _blockSize;
    std::list<int> _deallocated;

    std::vector<VERTEXDATA> _data;


};