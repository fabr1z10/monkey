#include "iquad.h"
#include "../components/quad_renderer.h"
#include "../pyhelper.h"
#include "../spritesheet.h"
#include "../math/random.h"

int Frame::getTicks() const {
	if (tickMin == -1) {
		return ticks;
	}
	return Random::instance().getUniform(tickMin, tickMin + maxTicks);

}

std::shared_ptr<Renderer> IQuad::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<SpriteRenderer>(args);

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

StaticQuad::    StaticQuad(const pybind11::kwargs& args) {
	_textureCoordinates = py_get_dict<glm::vec4>(args, "tex_coords");
	_size = py_get_dict<glm::vec2>(args, "size", glm::vec2(_textureCoordinates[2], _textureCoordinates[3]));
	_anchorPoint = py_get_dict<glm::vec2>(args, "anchor", glm::vec2(0.f));
	_repeat = py_get_dict<glm::vec2>(args, "repeat", glm::vec2(1.f, 1.f));
}

std::shared_ptr<Renderer> StaticQuad::getRenderer(const pybind11::kwargs & args) {
	return std::make_shared<StaticQuadRenderer>(args);
}