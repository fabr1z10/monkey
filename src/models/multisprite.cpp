#include "multisprite.h"
#include "../pyhelper.h"
#include <glm/gtx/transform.hpp>

using namespace pybind11::literals; // to bring in the `_a` literal

std::shared_ptr<Renderer> MultiSprite::getRenderer(const pybind11::kwargs& args) {

    auto batchId = py_get_dict<std::string>(args, "batch");
    return std::make_shared<MultiSpriteRenderer>(batchId, pybind11::kwargs());

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
    std::list<SpriteInfo> l;
    l.push_back({glm::vec2(0.f), -1, 0,0});
    auto f = m_node->getFlipX() ? -1.f : 1.f;
    while (!l.empty()) {
        auto info = l.front();
        l.pop_front();
        glm::vec2 offset(0.f);
        if (info.parent != -1) {
            const auto& frame = _renderers[info.parent]->getFrameInfo();
            if (!frame.links.empty() && info.link < frame.links.size()) {
                offset = (frame.links[info.link] - frame.anchor);
                offset.x *= f;
            }
        }
        glm::vec2 p = info.offset + offset;
        glm::mat4 t = glm::translate(glm::vec3(p, info.z));
        _renderers[info.spriteIndex]->setTransform(t);
        _renderers[info.spriteIndex]->update(dt);
        for (const auto& r : _multi->getNext(info.spriteIndex)) {
            l.push_back({p, info.spriteIndex, r.link, r.next, r.z});
        }
    }

}


void MultiSprite::addSprite(std::shared_ptr<Sprite> sprite, int parent, int link, float z) { //const pybind11::kwargs& args) {
    _next.emplace_back();
    //int parent = py_get_dict<int>(args, "parent", -1);

    if (parent != -1) {
        //int link = py_get_dict<int>(args, "link");
        //auto z = py_get_dict<float>(args, "z", 0.f);
        _next[parent].push_back(SubSpriteInfo(_sprites.size(), link, z));
    }

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


void MultiSpriteRenderer::setPalette(const std::string& palId) {
    for (auto& r : _renderers) r->setPalette(palId);
}