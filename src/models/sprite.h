#pragma once

#include <yaml-cpp/yaml.h>
#include "iquad.h"
#include "../shape.h"
#include "../hashpair.h"
//#include "spritesheet.h"
#include "../batch/quadbatch.h"

struct Frame {
	Frame() : ticks(0), boxId(-1), tickMin(-1), maxTicks(0), anchor(0.f) {
	}
	std::vector<QuadInfo> quads;
	int ticks;
	int tickMin;
	int maxTicks;
	int getTicks() const;
    glm::vec2 anchor;
	int boxId;
	std::vector<glm::vec2> links;
};

struct Animation {
	Animation () : loop(0) {}

	std::vector<Frame> frames;
	int loop;   // -1 no loop
	std::string next;
	pybind11::function _onEnd;
};



class Sprite : public Model {

public:
    Sprite(SpriteSheet* sheet, const YAML::Node& node);

	const Frame& getFrameInfo(const std::string& anim, int frame) const;

	const Animation* getAnimationInfo(const std::string& anim) const;

	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;

	//void initFromPy(const pybind11::kwargs&) override {}
    //void initFromYAML(const YAML::Node &node, const YAML::Node &globals) override;
	//void init(Node*) override;
	//void draw(Shader*, Node*) override;
	//std::shared_ptr<Renderer> getRenderer(IBatch*) override;
//	const FrameInfo& getFrameInfo(const std::string& anim, int frame);
//    const FrameInfo* getFrameInfo2(const std::string& anim, int frame);

    //const AnimInfo* getAnimInfo(const std::string& anim);
//
	std::shared_ptr<shapes::Shape> getShape (const std::string& anim, int frame) const;
//	std::shared_ptr<Shape> getShapeCast (const std::string& anim, int frame) const;
//	bool hasCollision(const std::string&) const;
//	//const SpriteCollisionInfo& getCollisionInfo(const std::string& anim) const;
//	std::shared_ptr<Model> generateDebugModel();
//	Bounds getStaticBounds() const;
////	void innerDraw(Shader*, const glm::mat4&, const std::string& key) override;
//	std::string getDefaultAnimation() const;
//	std::pair<int, int> getDebugShape(const std::string& anim, int frame);
//	std::pair<int, int> getDebugAttackShape(const std::string& anim, int frame);
//	Bounds getAttackRange() const;
	glm::vec2 getJoint(const std::string& anim, int frame, int joint) const;
	unsigned long getMaxBoxes() const;
	const float* getBoxData(const std::string& anim, int frame) const ;
	Bounds getStaticBounds() const;
	std::string getDefaultAnimation() const;
	int getQuadCount() const;
	void addFrameCallback(const std::string& s, int frame, pybind11::function f);
	void checkFrameCallback(const std::string& s, int frame, Node*);

private:
    //std::string _batchId;

	int _quadCount; // the number of quads to book -> this is the # pf quads of the frame with the highest n of quads

	std::unordered_map<std::string, Animation> _animations;

	std::string _defaultAnimation;

	//QuadBatch* _batch;
	Bounds m_attackRange;
	//SpriteSheet* m_sheet;
    //std::string m_defaultAnimation;
	//std::unordered_map<std::string, AnimInfo> m_animInfo;
	//std::unordered_map<std::string, std::vector<FrameInfo>> m_frameInfo;
	std::shared_ptr<shapes::Shape> getRect(int mode, int, int, int, int);
	float m_halfThickness;
	// collision shapes (if any)
	std::vector<std::shared_ptr<shapes::Shape>> m_shapes;
	std::unordered_map<std::pair<std::string, int>, int> m_frameToShape;
	Bounds m_collisionBounds;
	std::unordered_map<int, std::pair<int, int>> m_shapeInfo;

	std::vector<glm::vec2> m_joints;
    // joint override
    std::unordered_map<std::pair<std::string, int>, std::vector<glm::vec2>> m_jointOverride;
	std::vector<float> _boxData;
	std::vector<std::pair<unsigned, unsigned>> _boxOffset;

	size_t _maxBoxes;
	Bounds _staticBounds;
	std::unordered_map<std::pair<std::string, int>, pybind11::function> _callbacks;

};

inline int Sprite::getQuadCount() const {
	return 1;
}

inline std::string Sprite::getDefaultAnimation() const {
    return _defaultAnimation;
}

inline unsigned long Sprite::getMaxBoxes() const {
	return _maxBoxes;
}
//inline Bounds Sprite::getAttackRange() const {
//	return m_attackRange;
//}
//
//
//inline std::string Sprite::getDefaultAnimation() const {
//	return m_defaultAnimation;
//}
//
inline Bounds Sprite::getStaticBounds() const {
	return _staticBounds;
}
//
//inline const AnimInfo * Sprite::getAnimInfo(const std::string &anim) {
//	auto it = m_animInfo.find(anim);
//	if (it == m_animInfo.end())
//		return nullptr;
//
//	return &it->second;
//	//return &m_animInfo[anim];
//}
//
