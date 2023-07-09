#include "iquad.h"
#include "../components/sprite_renderer.h"



std::shared_ptr<Renderer> IQuad::getRenderer(IBatch* batch) {
    return std::make_shared<SpriteRenderer>(batch);

}

const Frame & IQuad::getFrameInfo(const std::string &anim, int frame) {
    return _animations.at(anim).frames[frame];
}

Animation * IQuad::getAnimationInfo(const std::string &anim) {
    return &_animations.at(anim);
}