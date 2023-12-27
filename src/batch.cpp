#include "batch.h"
#include "components/renderer.h"
#include "pyhelper.h"
#include "engine.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

IBatch::IBatch(int verticesPerElement, const pybind11::kwargs& args) : _vertsPerElement(verticesPerElement), _nPrimitive(0) {
	_maxElements = py_get_dict<int>(args, "max_elements");
	_camId = py_get_dict<int>(args, "cam");
	_cam = Engine::instance().getRoom()->getCamera(_camId);
}

ProvaBatch::ProvaBatch(const pybind11::kwargs &kwargs) : IBatch(0, kwargs) {
    _shaderType = SHADER_SKELETAL;
}

void ProvaBatch::addRenderer(Renderer * r) {
	_renderers[r->getNode()->getId()] = r;
}

void ProvaBatch::removeRenderer(Renderer *r) {
	_renderers.erase(r->getNode()->getId());
}

void ProvaBatch::draw(Shader *s) {
	for (const auto& renderer : _renderers) {

		renderer.second->draw(s);



	}

}
/*
	auto root = Engine::instance().getRoom()->getRoot();
	std::list<Node*> nodes{root.get()};

	while (!nodes.empty()) {
		auto* current = nodes.front();
		nodes.pop_front();
		auto renderer = current->getComponent<Renderer>();
		if (renderer != nullptr && renderer->getShaderType() == m_shaderType) {
			renderer->draw(this);
		}
//
//		for (const auto& children : current->getChildren()) {
//			nodes.push_back(children.second.get());
//		}
//
//	}
}
*/

void IBatch::setupUniforms(Shader* s) {
	// setup viewport and projection-view matrix
	auto vp = _cam->getViewport();
	glViewport(vp.x, vp.y, vp.z, vp.w);
	int jointMatrixLoc = glGetUniformLocation(s->getProgId(), "pv_mat");
	auto pvMatrix =  _cam->getProjectionMatrix() * _cam->getViewMatrix();
	glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(pvMatrix[0]));
}

//void IBatch::add(Node * node, const pybind11::kwargs& args) {
//    auto model = node->getModel();
//    auto renderer = model->getRenderer(this);
//    node->addComponent(renderer);
//	renderer->setNodeModel(model, args);
//
//}

int IBatch::getPrimitiveId() {
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

void IBatch::release(int id) {
    _deallocated.push_back(id);

}