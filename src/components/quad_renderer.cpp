#include "quad_renderer.h"
#include "../pyhelper.h"
#include "../spritesheet.h"



std::type_index StaticQuadRenderer::getType() {
	return std::type_index(typeid(Renderer));
}

void StaticQuadRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	//m_sprite = std::dynamic_pointer_cast<StaticQuad>(model);
	// only 1 quad
	_model = dynamic_cast<StaticQuad*>(model.get());

	_primitiveIds.push_back(_batch->getPrimitiveId());
	_paletteId = py_get_dict<int>(args, "pal", 0);
}

void StaticQuadRenderer::start() {

}

