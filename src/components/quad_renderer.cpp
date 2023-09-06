#include "quad_renderer.h"
#include "../pyhelper.h"
#include "../spritesheet.h"

StaticQuadRenderer::StaticQuadRenderer(const pybind11::kwargs &args) : BatchRenderer<QuadBatch>(args) {
	assert(_batch != nullptr);
	_paletteId = py_get_dict<unsigned>(args, "pal", 0);
	_camId = py_get_dict<unsigned>(args, "cam", 0);
}

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
	auto worldTransform = m_node->getWorldMatrix();

	auto bottomLeft = worldTransform * glm::vec4(m_shift - glm::vec3(_model->getAnchorPoint(), 0.f), 1.f);
	auto size = _model->getSize();

	auto tex = _batch->getSheet()->getTex();
	glm::vec4 texCoords = _model->getTexCoords();

	auto tw = tex->getWidth();
	auto th = tex->getHeight();

	float tx0 = texCoords[0] / tw;
	float tx1 = (texCoords[0] + texCoords[2]) / tw;
	float ty0 = texCoords[1] / th;
	float ty1 = (texCoords[1] + texCoords[3]) / th;




 	_batch->setQuad(_primitiveIds[0],
					bottomLeft,
					_model->getSize(),
					glm::vec4(tx0, tx1, ty0, ty1),
					_model->getRepeat(),
					_paletteId,
					false,
					false,
					0.f);
}