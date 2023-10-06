#pragma once

#include "../model.h"
#include <pybind11/pybind11.h>
#include "../batch/quadbatch.h"

const int MAX_JOINTS = 2;
// describe a single quad
struct Desc {
	Desc() : location(0.f), anchorPoint(0.f), size(0.f), repeat(1.f), textureCoordinates(0.f), flipv(false), fliph(false) {}
	glm::vec3 location;
    glm::vec2 anchorPoint;
    glm::vec2 size;
    glm::vec2 repeat;
    glm::vec4 textureCoordinates;
    float advance;
    bool flipv;
    bool fliph;
    //unsigned paletteIndex;
};

struct Frame {
    Frame() : ticks(0), boxId(-1), tickMin(-1), maxTicks(0) {
        for (int i = 0; i < MAX_JOINTS; ++i) {
            joints[i] = glm::vec2(0.f, 0.f);
        }
    }
    std::vector<Desc> quads;
    int ticks;
    int tickMin;
    int maxTicks;
	int getTicks() const;

    int boxId;
    std::array<glm::vec2, MAX_JOINTS> joints;
};

struct Animation {
    Animation () : loop(0) {}

    std::vector<Frame> frames;
    int loop;   // -1 no loop
    std::string next;
};

class SpriteSheet;

class IQuad : public Model {
public:
    IQuad() : _quadCount(0) {}
    virtual ~IQuad() {}
    // returns information for rendering a frame (used by the renderer - which is responsibile
    // for keeping track of current animation and frame
    const Frame& getFrameInfo(const std::string& anim, int frame);
    Animation* getAnimationInfo(const std::string& anim);
    //virtual void initFromPy(const pybind11::kwargs&) = 0;
    //virtual void initFromYAML(const YAML::Node &node, const YAML::Node &globals) = 0;
    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
    std::string getDefaultAnimation() const;
    int getQuadCount() const;
protected:
    std::unordered_map<std::string, Animation> _animations;
    int _quadCount; // the number of quads to book -> this is the # pf quads of the frame with the highest n of quads
    std::string _defaultAnimation;
    //QuadBatch* _batch;
	//SpriteSheet* _sheet;
};

inline std::string IQuad::getDefaultAnimation() const {
    return _defaultAnimation;
}

inline int IQuad::getQuadCount() const {
    return _quadCount;
}


class StaticQuad : public Model {
public:
	StaticQuad(const pybind11::kwargs&);
	glm::vec2 getAnchorPoint() const;
	glm::vec2 getSize() const;
	glm::vec4 getTexCoords() const;
	glm::vec2 getRepeat() const;
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;

private:
	glm::vec2 _anchorPoint;
	glm::vec2 _size;
	glm::vec4 _textureCoordinates;
	glm::vec2 _repeat;

};


inline glm::vec2 StaticQuad::getAnchorPoint() const {
	return _anchorPoint;
}
inline glm::vec2 StaticQuad::getSize() const {
	return _size;
}

inline glm::vec4 StaticQuad::getTexCoords() const {
	return _textureCoordinates;
}

inline glm::vec2 StaticQuad::getRepeat() const {
	return _repeat;
}