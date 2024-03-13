#include "multisprite.h"
#include "../pyhelper.h"
#include <glm/gtx/transform.hpp>

using namespace pybind11::literals; // to bring in the `_a` literal

std::shared_ptr<Renderer> MultiSprite::getRenderer(const pybind11::kwargs& args) {

    auto batchId = py_get_dict<std::string>(args, "batch");
    return std::make_shared<MultiSpriteRenderer>(batchId);

}

std::vector<std::shared_ptr<Sprite>> & MultiSprite::getSprites() {
    return _sprites;
}

std::vector<AnimationData> * MultiSprite::getAnimationData(const std::string &id) {
    auto it = _animations.find(id);
    if (it == _animations.end()) {
        return nullptr;
    }
    return &it->second;
}

void MultiSprite::addAnimation(const std::string &animId) {
    _animations[animId] = std::vector<AnimationData>(_sprites.size(), AnimationData(animId));

}

void MultiSprite::setAnimationData(const std::string &animId, int spriteId, const std::string &subAnim,
                                   glm::vec3 offset) {
    auto& a = _animations[animId][spriteId];
    a.animation = subAnim;
    a.offset = offset;
}

void MultiSpriteRenderer::start() {
    for (const auto& renderer : _renderers) {
        renderer->setNode(m_node);
        renderer->start();
    }
}

void MultiSpriteRenderer::update(double dt) {
    // simply update all renderers
    for (const auto& renderer : _renderers) {
        renderer->update(dt);
    }
}


void MultiSprite::addSprite(const std::shared_ptr<Sprite> sprite) {
    _sprites.push_back(sprite);
}


void MultiSpriteRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
    Renderer::setModel(model, args);
    _multi = std::dynamic_pointer_cast<MultiSprite>(model).get();
    for (auto& sprite : _multi->getSprites()) {
        auto renderer = std::dynamic_pointer_cast<SpriteRenderer>(sprite->getRenderer(args));
        renderer->setModel(sprite, args); //pybind11::dict("batch"_a = _batchId));
        _renderers.push_back(renderer);
    }
}

void MultiSpriteRenderer::setAnimation(const std::string & anim) {
    auto* data = _multi->getAnimationData(anim);
    if (data == nullptr) {
        for (auto &r : _renderers) {
            r->setAnimation(anim);
        }
    } else {
        for (size_t i =0; i< _renderers.size(); ++i) {
            _renderers[i]->setAnimation((*data)[i].animation);
            _renderers[i]->setTransform(glm::translate((*data)[i].offset));
        }

    }

}

std::type_index MultiSpriteRenderer::getType() {
    return std::type_index(typeid(Renderer));
}

