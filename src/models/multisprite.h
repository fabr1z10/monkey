#pragma once


#include "sprite.h"
#include "../components/sprite_renderer.h"

struct AnimationData {
    explicit AnimationData(const std::string& anim) : animation(anim), offset(glm::vec3(0.f)) {}
    std::string animation;
    glm::vec3 offset;
};

class MultiSprite : public Model {
public:


    MultiSprite() : Model() {}
    void addSprite(const std::shared_ptr<Sprite> sprite);
    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
    std::vector<std::shared_ptr<Sprite>>& getSprites();
    void addAnimation(const std::string& animId);
    void setAnimationData(const std::string& animId, int spriteId, const std::string& subAnim, glm::vec3 offset);
    std::vector<AnimationData>* getAnimationData(const std::string& id);
private:
    std::unordered_map<std::string, std::vector<AnimationData>> _animations;
    std::vector<std::shared_ptr<Sprite>> _sprites;
};

class MultiSpriteRenderer : public BatchRenderer<QuadBatch> {
public:
    MultiSpriteRenderer(const std::string& batchId) : BatchRenderer<QuadBatch>(batchId), _multi(nullptr) {}
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
    void setAnimation(const std::string&) override;
    void start() override;
    void update(double) override;
    std::type_index getType() override;

private:
    // compound animations
    std::vector<std::shared_ptr<SpriteRenderer>> _renderers;
    MultiSprite* _multi;
};