#include "sprite_collider.h"
#include "../node.h"
#include "../engine.h"
#include "../pyhelper.h"
#include "../runners/collision_engine.h"
#include "../models/sprite.h"

SpriteCollider::SpriteCollider(int flag, int mask, int tag, const pybind11::kwargs& args) : Collider(flag, mask, tag),
	m_sprite(nullptr), m_renderer(nullptr) {
	m_castMask = py_get_dict<int>(args, "cast_mask", 0);
	m_castTag = py_get_dict<int>(args, "cast_tag", 0);

}

void SpriteCollider::start() {
	// a node that has a spritecollider needs to have a sprite model (and a sprite renderer)
	m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
	assert(m_renderer != nullptr);

//	m_sprite = m_renderer->getSprite();
//	m_staticBounds = m_sprite->getStaticBounds();
	Collider::start();
}

void SpriteCollider::update(double) {
	// check if current (anim, frame) has a shape to cast
//	auto animId = m_renderer->getAnimation();
//	auto frame = m_renderer->getFrame();
//	auto box = m_sprite->getShapeCast(animId, frame);
//	bool hit = false;
//	if (box != nullptr) {
//		auto t = m_node->getWorldMatrix();
//		auto e = m_engine->shapeCast(box.get(), t, m_castMask, true);
//		if (!e.empty()) {
//			hit = true;
//			m_engine->processCollisions(e, m_node, m_castTag);
//		}
//	}
//	if (!hit) {
//		// m_lastHit = nullptr;
//	}




}


std::shared_ptr<Shape> SpriteCollider::getShape() {
	//return m_sprite->getShape(m_renderer->getAnimation(), m_renderer->getFrame());
}

void SpriteCollider::generateDebugMesh() {
//	if (m_debugNode != nullptr) {
//		m_debugNode->remove();
//	}
//	auto model = m_sprite->generateDebugModel();
//
	auto node = std::make_shared<Node>();
//	node->setNodeModel(model);
	auto renderer = std::make_shared<SpriteColliderRenderer>();
//	renderer->setNodeModel(model);
	node->addComponent(renderer);
	m_node->add(node);
//	m_debugNode = node.get();
}


std::type_index SpriteColliderRenderer::getType() {
	return std::type_index(typeid(Renderer));
}

SpriteColliderRenderer::SpriteColliderRenderer() : Renderer() {
	_lineBatch = Engine::instance().getRoom()->getLineBatch();

}

void SpriteColliderRenderer::start() {
//	Renderer::start();
//	m_reference = dynamic_cast<SpriteRenderer*>(m_node->getParent()->getComponent<Renderer>());
//	m_sprite = m_reference->getSprite();
//	assert(m_reference!=nullptr);
}

void SpriteColliderRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	m_sprite = std::dynamic_pointer_cast<Sprite>(model).get();
	auto qc = m_sprite->getMaxBoxes();
	for (int i = 0; i < qc; ++i) {
		_quadIds.push_back(_lineBatch->getPrimitiveId());
	}
	//m_animation = py_get_dict<std::string>(args, "animation", m_sprite->getDefaultAnimation());
	//_paletteId = py_get_dict<int>(args, "pal", 0);
}

//void SpriteColliderRenderer::draw(Shader * s) {
//	if (m_model == nullptr || s->getShaderType() != ShaderType::SHADER_COLOR) {
//		return;
//	}
//	const auto& m = m_node->getWorldMatrix() ;
//	s->setVec4("mult_color", m_multColor);
//	s->setVec4("add_color", m_addColor);
//	s->setMat4("model", m);
//	const auto& currentAnim = m_reference->getAnimation();
//	int currentFrame = m_reference->getFrame();
//	auto data = m_sprite->getDebugShape(currentAnim, currentFrame);
//	auto dataA = m_sprite->getDebugAttackShape(currentAnim, currentFrame);
//	if (data.first != -1) {
//		m_model->draw(s, data.first, data.second);
//	}
//	if (dataA.first != -1) {
//		m_model->draw(s, dataA.first, dataA.second);
//	}
//
//
//}

int SpriteCollider::getCollisionTag() const {
	auto it = m_override.find(m_renderer->getAnimation());
	if (it != m_override.end()) {
		return it->second[2];
	}
	return m_tag;
}

int SpriteCollider::getCollisionFlag() const {
	auto it = m_override.find(m_renderer->getAnimation());
	if (it != m_override.end()) {
		return it->second[0];
	}
	return m_flag;

}

int SpriteCollider::getCollisionMask() const {
	auto it = m_override.find(m_renderer->getAnimation());
	if (it != m_override.end()) {
		return it->second[1];
	}
	return m_mask;

}

void SpriteCollider::setCollisionOverride(const std::string & id , int flag, int mask, int tag) {
	m_override[id] = glm::ivec3(flag, mask, tag);
}