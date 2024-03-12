#pragma once


#include "sprite.h"
#include "../components/sprite_renderer.h"

class MultiSprite : public Model {
public:
    MultiSprite();
    void addSprite(const std::shared_ptr<Sprite> sprite);
    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
    std::vector<std::shared_ptr<Sprite>>& getSprites();
private:
    std::vector<std::shared_ptr<Sprite>> _sprites;
};

class MultiSpriteRenderer : public BatchRenderer<QuadBatch> {
public:
    MultiSpriteRenderer(const std::string& args);
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
    void setAnimation(const std::string&) override;

    void update(double) override;
private:
    std::string _batchId;
    // compound animations
    std::vector<std::shared_ptr<SpriteRenderer>> _renderers;
    MultiSprite* _multi;
};