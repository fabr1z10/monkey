#pragma once

#include "collider.h"
#include "sprite_renderer.h"
#include "../batch/linebatch.h"

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
    const Sprite* m_sprite;
    void generateDebugMesh() override;
};


class SpriteColliderRenderer : public BatchRenderer<LineBatch> {
public:
    SpriteColliderRenderer(const std::string& batchId);
    //void draw(Shader * s) override;
    void start() override;
    std::type_index getType() override;
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
    void update(double) override;

private:
    SpriteRenderer* m_reference;
    const Sprite* m_sprite;
    std::string _batchId;


};