#pragma once

#include <yaml-cpp/yaml.h>
#include "iquad.h"
#include "../shape.h"
#include "../hashpair.h"
//#include "spritesheet.h"
#include "../batch/quadbatch.h"


class Sprite : public IQuad {

public:
    Sprite(SpriteSheet* sheet, const YAML::Node& node);
    //void initFromPy(const pybind11::kwargs&) override {}
    //void initFromYAML(const YAML::Node &node, const YAML::Node &globals) override;
	//void init(Node*) override;
	//void draw(Shader*, Node*) override;
	//std::shared_ptr<Renderer> getRenderer(IBatch*) override;
//	const FrameInfo& getFrameInfo(const std::string& anim, int frame);
//    const FrameInfo* getFrameInfo2(const std::string& anim, int frame);

    //const AnimInfo* getAnimInfo(const std::string& anim);
//
	std::shared_ptr<Shape> getShape (const std::string& anim, int frame) const;
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
	ulong getMaxBoxes() const;
	const float* getBoxData(const std::string& anim, int frame);
	Bounds getStaticBounds() const;
private:
    //QuadBatch* _batch;
	Bounds m_attackRange;
	//SpriteSheet* m_sheet;
    //std::string m_defaultAnimation;
	//std::unordered_map<std::string, AnimInfo> m_animInfo;
	//std::unordered_map<std::string, std::vector<FrameInfo>> m_frameInfo;
	std::shared_ptr<Shape> getRect(int mode, int, int, int, int);
	float m_halfThickness;
	// collision shapes (if any)
	std::vector<std::shared_ptr<Shape>> m_shapes;
	std::unordered_map<std::pair<std::string, int>, int> m_frameToShape;
	Bounds m_collisionBounds;
	std::unordered_map<int, std::pair<int, int>> m_shapeInfo;

	std::vector<glm::vec2> m_joints;
    // joint override
    std::unordered_map<std::pair<std::string, int>, std::vector<glm::vec2>> m_jointOverride;
	std::vector<float> _boxData;
	std::vector<std::pair<unsigned, unsigned>> _boxOffset;
	ulong _maxBoxes;
	Bounds _staticBounds;
};

inline ulong Sprite::getMaxBoxes() const {
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
