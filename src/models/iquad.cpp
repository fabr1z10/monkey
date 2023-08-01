#include "iquad.h"
#include "../components/sprite_renderer.h"

#include "../spritesheet.h"

std::shared_ptr<Renderer> IQuad::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<SpriteRenderer>(_sheet->getBatch().get(), args);

}

const Frame & IQuad::getFrameInfo(const std::string &anim, int frame) {
    return _animations.at(anim).frames[frame];
}

Animation * IQuad::getAnimationInfo(const std::string &anim) {
    auto it = _animations.find(anim);
    if (it == _animations.end())
        return nullptr;
    return &it->second;
}