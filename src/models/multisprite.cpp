#include "multisprite.h"
#include "../pyhelper.h"


std::shared_ptr<Renderer> MultiSprite::getRenderer(const pybind11::kwargs& args) {

    auto batchId = py_get_dict<std::string>(args, "batch");
    return std::make_shared<MultiSpriteRenderer>(batchId);

}

void MultiSpriteRenderer::update(double dt) {
    // simply update all renderers
    for (const auto& renderer : _renderers) {
        renderer->update(dt);
    }
}

void MultiSpriteRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
    Renderer::setModel(model, args);
    _multi = std::dynamic_pointer_cast<MultiSprite>(model).get();
    for (auto& sprite : _multi->getSprites()) {
        auto renderer = std::dynamic_pointer_cast<SpriteRenderer>(sprite->getRenderer(args));
        renderer->setModel(sprite, args);
        _renderers.push_back(renderer);
    }
}

void MultiSpriteRenderer::setAnimation(const std::string &) {


}