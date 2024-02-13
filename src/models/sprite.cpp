#include "sprite.h"
#include "../assetmanager.h"
#include <iostream>
#include "../engine.h"

#include "../symbols.h"
#include "../components/sprite_renderer.h"
#include "../pyhelper.h"
#include "../shapes/aabb.h"
#include "../shapes/compound.h"
//#include "../modelmake.h"
//#include "rawmodel.h"
//#include "../node.h"
//#include "../shapes/shapes3d/aabb3d.h"
#include "../yamlexp.h"
#include "../spritesheet.h"

// construct from YAML
Sprite::Sprite(SpriteSheet* sheet, const YAML::Node& node) : Model() {

	//_batch = Engine::instance().getRoom()->addSpriteBatch(sheet);

	auto& am = AssetManager::instance();


	auto * tex = sheet->getTex();
	float texw = tex->getWidth();
	float texh = tex->getHeight();

	auto& engine = Engine::instance();

	auto defaultTicks = node["ticks"].as<int>(10);
	float z = node["z"].as<float>(0.f);
	// read collision boxes
	_maxBoxes = 0;
	_staticBounds = Bounds();
	for (const auto& n : node["boxes"]) {
		auto a = n.as<std::vector<float>>();
		assert(a.size() % 4 == 0);
		_maxBoxes = std::max(_maxBoxes, a.size() / 4);
		_boxOffset.emplace_back(_boxData.size(), a.size() / 4);
		_boxData.insert(_boxData.end (), a.begin(), a.end ());
		for (size_t i = 0; i < a.size(); i+=4) {
		    _staticBounds.min.x = std::min(_staticBounds.min.x, a[i]);
            _staticBounds.max.x = std::max(_staticBounds.max.x, a[i+2]);
            _staticBounds.min.y = std::min(_staticBounds.min.y, a[i+1]);
            _staticBounds.max.y = std::max(_staticBounds.max.y, a[i+3]);
		}
		// create a shape
		if (a.size() == 4) {
		    m_shapes.push_back(std::make_shared<AABB>(a[0], a[2], a[1], a[3]));
		}
	}


//	for (YAML::const_iterator anit = node["boxes"].begin(); anit != node["boxes"].end(); ++anit) {
//		auto a = (*anit).as<std::vector<float>>();
//		assert(a.size() % 4 == 0);
//		std::shared_ptr<Shape> shape;
//		if (a.size() == 4) {
//			shape = getRect(mode, a[0], a[1], a[2], a[3]);
//		} else {
//			auto s1 = std::make_shared<CompoundShape>();
//			for (size_t i = 0; i < a.size(); i+=4) {
//				s1->addShape(getRect(mode, a[i], a[i+1], a[i+2], a[i+3]));
//			}
//			shape = s1;
//		}
//		m_collisionBounds.expandWith(shape->getBounds());
//		m_shapes.push_back(shape);
//	}


	//std::vector<float> vertices;
	//std::vector<unsigned> indices;
	float ppu{1.f};
	int quadCount {0};
	auto inf = std::numeric_limits<float>::max();
	m_modelBounds.min = glm::vec3(inf, inf, -100.f);
	m_modelBounds.max = glm::vec3(-inf, -inf, 100.f);

	int defaultBox = -1;

	for (const auto& py_anim : node["animations"]) {
		auto animId = py_anim.first.as<std::string>();
		auto a = py_anim.second;
		if (_defaultAnimation.empty()) _defaultAnimation = animId;

		Animation animInfo;
		std::vector<Frame> frameInfos;
		animInfo.loop = a["loop"].as<int>(0);
		animInfo.next = a["next"].as<std::string>("");

		auto onEnd = a["on_end"].as<std::string>("");
		if (!onEnd.empty()) {
			animInfo._onEnd = Engine::instance().getScript(onEnd);
		}
		defaultBox = a["box"].as<int>(-1);
		//animInfo.loopFrame = anit->second["loop_frame"].as<int>(0);
		//int boxAnim = anit->second["box"].as<int>(-1);
		//animInfo.frameCount = 0;
		int frameCount = 0;

		for (const auto& el : a["frames"]) {
			Frame frameInfo;
			//frameInfo.offset = indices.size();
			//frameInfo.count = 0;
			auto loopFrame = el["loop_frame"].as<bool>(false);
			if (loopFrame) {
				animInfo.loop = frameCount;
			}
			frameInfo.ticks = el["ticks"].as<int>(defaultTicks);
			frameInfo.tickMin = el["tick_min"].as<int>(-1);
			frameInfo.maxTicks = el["max_ticks"].as<int>(0);
			frameInfo.boxId = el["box"].as<int>(defaultBox);
			//int boxFrame = el["box"].as<int>(boxAnim);
			//bool fliph = el["fliph"].as<bool>(false);
			//bool flipv = el["flipv"].as<bool>(false);
			// TODO restore attack
			//  frameInfo.box = boxFrame;
			//frameInfo.attackBox = el["attack"].as<int>(-1);
			//if (frameInfo.attackBox != -1)\ {
			//	m_attackRange.expandWith(m_shapes[frameInfo.attackBox]->getBounds());
			//}
			//m_frameToShape[std::make_pair(animId, frameCount)] = boxFrame;
			int quadCurrentFrame {0};
			//for (const auto& q : el["quads"]) {
			QuadInfo desc;
            auto texc = el["tex"].as<glm::vec4>();
            int width_px = texc[2];
            int height_px = texc[3];
			//desc.paletteIndex = yaml_read<int>(q, "palette", 0);
			desc.fliph = el["fliph"].as<bool>(false);
            desc.flipv = el["flipv"].as<bool>(false);
            desc.palette = el["pal"].as<int>(0);
            desc.textureCoordinates[0] = texc[0] / texw;
            desc.textureCoordinates[1] = (texc[0] + texc[2]) / texw;
            desc.textureCoordinates[2] = texc[1] / texh;
            desc.textureCoordinates[3] = (texc[1] + texc[3]) / texh;
            if (desc.fliph) {
                std::swap(desc.textureCoordinates[0], desc.textureCoordinates[1]);
            }
            auto anchor = el["anchor"].as<glm::vec2>(glm::vec2(0.f));
			desc.repeat = el["repeat"].as<glm::vec2>(glm::vec2(1.f, 1.f));
            desc.location = glm::vec3(-anchor.x, -anchor.y, 0.f);
            //desc.location = el["pos"]. py_get_dict<glm::vec3>(el, "pos", glm::vec3(0.f));
            float width_actual = static_cast<float>(width_px) / ppu;
            float height_actual = static_cast<float>(height_px) / ppu;
            desc.size = glm::vec2(width_actual, height_actual);
            m_modelBounds.min.x = std::min(m_modelBounds.min.x, desc.location.x);
            m_modelBounds.min.y = std::min(m_modelBounds.min.y, desc.location.y);
            m_modelBounds.max.x = std::max(m_modelBounds.max.x, desc.location.x + width_actual);
            m_modelBounds.max.y = std::max(m_modelBounds.max.y, desc.location.y + height_actual);
            frameInfo.quads.push_back(desc);
            quadCurrentFrame++;
			_quadCount = 1;
            if (el["joints"]) {
                int jointIndex {0};
                for (auto joint : el["joints"]) {
                    frameInfo.joints[jointIndex++] = joint.as<glm::vec2>();
                    if (jointIndex >= MAX_JOINTS) break;
                }

                //m_jointOverride[std::make_pair(animId, frameCount)] = py_get_dict<std::vector<glm::vec2>>(el, "joints");
            }

			frameCount++;
			animInfo.frames.push_back(frameInfo);
		}
		_animations.insert(std::make_pair(animId, animInfo));
	}
	//generateBuffers(vertices, indices);


}


const Frame & Sprite::getFrameInfo(const std::string &anim, int frame) const {
    return _animations.at(anim).frames[frame];
}

const Animation * Sprite::getAnimationInfo(const std::string &anim) const {
    auto it = _animations.find(anim);
    if (it == _animations.end())
        return nullptr;
    return &it->second;
}

std::shared_ptr<Renderer> Sprite::getRenderer(const pybind11::kwargs& args) {

    auto batchId = py_get_dict<std::string>(args, "batch");
	return std::make_shared<SpriteRenderer>(batchId);

}
//
//void Sprite::init(Node * n) {
//	auto obj = n->getUserData();
//	obj["anim"] = m_defaultAnimation;
//	obj["frame"] = 0;
//}

//Sprite::Sprite(IBatch* batch, ShaderType type, GLenum primitive) : Model() {
//
//	//m_primitive = primitive;
//}


std::shared_ptr<Shape> Sprite::getRect(int mode, int x0, int x1, int y0, int y1) {
	if (mode == 0) {
		return std::make_shared<AABB>(x0, x1, y0, y1);
	} else {
		//return std::make_shared<AABB3D>(x0, x1, y0, y1, -m_halfThickness, m_halfThickness);

		return nullptr;
	}

}



//const FrameInfo & Sprite::getFrameInfo(const std::string &anim, int frame) {
//	return m_animInfo.at(anim).frameInfo[frame];
//}

//std::shared_ptr<Renderer> Sprite::getRenderer(IBatch* batch) {
//	return std::make_shared<SpriteRenderer>(batch);
//
//}


//
//bool Sprite::hasCollision(const std::string & anim) const {
//	return m_frameToShape.count(std::make_pair(anim, 0)) > 0;
//
//}

std::shared_ptr<Shape> Sprite::getShape (const std::string& anim, int frame) const {
    return m_shapes[_animations.at(anim).frames[frame].boxId];
}
//	auto it = m_frameToShape.find(std::make_pair(anim, frame));
//	if (it == m_frameToShape.end())
//		return nullptr;
//	if (it->second == -1)
//		return nullptr;
//	return m_shapes[it->second];
//}

//std::shared_ptr<Shape> Sprite::getShapeCast (const std::string& anim, int frame) const {
//	const auto& a = m_animInfo.at(anim).frameInfo[frame];
//	if (a.attackBox == -1)
//		return nullptr;
//	return m_shapes[a.attackBox];
//}
//
//std::pair<int, int> Sprite::getDebugShape(const std::string &anim, int frame) {
//	int boxId = m_animInfo.at(anim).frameInfo[frame].box;
//	if (boxId == -1) {
//		return std::make_pair(-1, -1);
//	}
//	return m_shapeInfo[boxId];
//}
//
//std::pair<int, int> Sprite::getDebugAttackShape(const std::string &anim, int frame) {
//	int boxId = m_animInfo.at(anim).frameInfo[frame].attackBox;
//	if (boxId == -1) {
//		return std::make_pair(-1, -1);
//	}
//	return m_shapeInfo[boxId];
//}


//std::shared_ptr<Model> Sprite::generateDebugModel() {
////	std::vector<float> vertices;
////	std::vector<unsigned> elements;
////	unsigned u{0};
////	glm::vec4 color(1.f);
////	auto model = std::make_shared<Model>();
//////
//////    auto model = std::make_shared<AnimatedModel>(ShaderType::SHADER_COLOR, GL_LINES);
//////
////	// every shape is associated an offset and a count
////
////	auto lambda = [&] (const Bounds& b) {
////		vertices.insert(vertices.end(), {b.min.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
////		vertices.insert(vertices.end(), {b.max.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
////		vertices.insert(vertices.end(), {b.max.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
////		vertices.insert(vertices.end(), {b.min.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
////		elements.insert(elements.end(), {u, u+1, u+1, u+2, u+2, u+3, u+3, u});
////		u += 4;
////	};
////
////	int ishape{0};
////	for (const auto& s : m_shapes) {
////		unsigned offset = elements.size();
////		if (s->getShapeType() == ShapeType::COMPOUND) {
////			auto* cs = static_cast<CompoundShape*>(s.get());
////			for (const auto& t : cs->getShapes()) {
////				lambda(t->getBounds());
////			}
////		} else {
////			lambda(s->getBounds());
////		}
////		m_shapeInfo[ishape++] = std::make_pair(offset, elements.size() - offset);
//////		model->addItem(offset, elements.size() - offset);
////	}
////	model->generateBuffers(vertices, elements);
////
////	return model;
////	//return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);
//
//}
//
glm::vec2 Sprite::getJoint(const std::string &anim, int frame, int joint) const {
    return _animations.at(anim).frames[frame].joints[joint];
//    auto it = m_jointOverride.find(std::make_pair(anim, frame));
//    if (it == m_jointOverride.end()) {
//        return m_joints[joint];
//    }
//    return it->second[joint];
}

const float *Sprite::getBoxData(const std::string &anim, int frame) const {
    auto boxId = _animations.at(anim).frames[frame].boxId;
    if (boxId == -1) {
        return nullptr;
    }
	auto& p = _boxOffset[boxId];
	return &_boxData[p.first];
}