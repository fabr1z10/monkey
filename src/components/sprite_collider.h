#pragma once

#include "collider.h"
#include "sprite_renderer.h"

class SpriteCollider : public Collider {
public:
	SpriteCollider(int flag, int mask, int tag, const pybind11::kwargs& args);
	std::shared_ptr<Shape> getShape() override;
	void setCollisionOverride(const std::string&, int, int, int);
	void start() override;
	void update(double) override;
	int getCollisionTag() const override;
	int getCollisionFlag() const override;
	int getCollisionMask() const override;

private:
	std::unordered_map<std::string, glm::ivec3> m_override;
	int m_castMask;
	int m_castTag;
	SpriteRenderer* m_renderer;
	Sprite* m_sprite;
	void generateDebugMesh() override;
};


class SpriteColliderRenderer : public Renderer {
public:
	SpriteColliderRenderer() : Renderer(GL_INVALID_VALUE, GL_INVALID_VALUE) {}
	//void draw(Shader * s) override;
	void start() override;
	std::type_index getType() override;
private:
	SpriteRenderer* m_reference;
	Sprite* m_sprite;
};
