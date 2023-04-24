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
#include "../input.h"

Sprite::Sprite(const YAML::Node& node) : Model(), m_defaultAnimation(std::string()), _batch(nullptr) {
	m_shaderType = ShaderType::SHADER_TEXTURE;
	m_primitive = GL_TRIANGLES;
	auto& am = AssetManager::instance();

	auto batch = yaml_read<int>(node, "batch");
	_batch = Engine::instance().getBatch(batch);
	auto sheetFile = _batch->getSheet();
	//auto sheetFile = node["sheet"].as<std::string>();
	auto tex = am.getTex(sheetFile);

	if (tex->hasPalette()) {
		m_shaderType = ShaderType::SHADER_TEXTURE_PALETTE;
		m_paletteId = tex->getDefaultPaletteId();
//		auto paletteName = node["palette"].as<std::string>("");
//		if (!paletteName.empty()) {
//			auto pal = am.getPalette(paletteName);
//			m_paletteId = pal->getTexId();
//		}
	}

	float texw = tex->getWidth();
	float texh = tex->getHeight();
	m_texId = tex->getTexId();

	auto& engine = Engine::instance();

	// mode = 0 --> 2D, mode = 1 --> 3D

	auto mode = py_get<int>(engine.getConfig(),"game_mode", 0);
	m_halfThickness = (mode == 0) ? 0 : node["thickness"].as<float>();
	auto defaultTicks = py_get<int>(engine.getConfig(),"ticks", 5);

	// read collision boxes
	for (YAML::const_iterator anit = node["boxes"].begin(); anit != node["boxes"].end(); ++anit) {
		auto a = (*anit).as<std::vector<float>>();
		assert(a.size() % 4 == 0);
		std::shared_ptr<Shape> shape;
		if (a.size() == 4) {
			shape = getRect(mode, a[0], a[1], a[2], a[3]);
		} else {
			auto s1 = std::make_shared<CompoundShape>();
			for (size_t i = 0; i < a.size(); i+=4) {
				s1->addShape(getRect(mode, a[i], a[i+1], a[i+2], a[i+3]));
			}
			shape = s1;
		}
		m_collisionBounds.expandWith(shape->getBounds());
		m_shapes.push_back(shape);
	}


	//std::vector<float> vertices;
	//std::vector<unsigned> indices;
	float ppu{1.f};
	int quadCount {0};
	auto inf = std::numeric_limits<float>::max();
	m_modelBounds.min = glm::vec3(inf, inf, -100.f);
	m_modelBounds.max = glm::vec3(-inf, -inf, 100.f);

	for (YAML::const_iterator anit = node["animations"].begin(); anit != node["animations"].end(); ++anit) {
		auto animId = anit->first.as<std::string>();
		if (m_defaultAnimation.empty()) m_defaultAnimation = animId;
		//std::cerr << " reading animation:" << animId << "\n";
		AnimInfo animInfo;
		std::vector<FrameInfo> frameInfos;
		animInfo.loop = anit->second["loop"].as<bool>(true);
		//animInfo.loopFrame = anit->second["loop_frame"].as<int>(0);
		int boxAnim = anit->second["box"].as<int>(-1);
		animInfo.frameCount = 0;
		int frameCount = 0;
		for (const auto& el : anit->second["frames"]) {
			FrameInfo frameInfo;
			//frameInfo.offset = indices.size();
			//frameInfo.count = 0;
			auto loopFrame = el["loop_frame"].as<bool>(false);
			if (loopFrame) {
				animInfo.loopFrame = frameCount;
			}
			frameInfo.ticks = el["ticks"].as<int>(defaultTicks);
			int boxFrame = el["box"].as<int>(boxAnim);
			bool fliph = el["fliph"].as<bool>(false);
			bool flipv = el["flipv"].as<bool>(false);
			frameInfo.box = boxFrame;
			frameInfo.attackBox = el["attack"].as<int>(-1);
			if (frameInfo.attackBox != -1) {
				m_attackRange.expandWith(m_shapes[frameInfo.attackBox]->getBounds());
			}
			m_frameToShape[std::make_pair(animId, frameCount)] = boxFrame;
			auto ciao = el["quads"].as<std::vector<int>>();
			assert(ciao.size() % 6 == 0);
			int nquads = ciao.size() / 6;
			for (size_t i = 0; i < ciao.size(); i += 6) {
				int width_px = ciao[i+2];
				int height_px = ciao[i+3];
				float tx = ciao[0] / texw;
				float ty = ciao[1] / texh;
				float tw = width_px / texw;
				float th = height_px / texh;
				frameInfo.texture_coordinates = glm::vec4(tx, tx + tw, ty, ty + th);
				float tx1 = tx + tw;
				float ty1 = ty + th;
				float ox = ciao[4];
				float oy = ciao[5];
				float width_actual = static_cast<float>(width_px) / ppu;
				float height_actual = static_cast<float>(height_px) / ppu;
                frameInfo.size = glm::vec2(width_actual, height_actual);
				if (fliph) std::swap(tx, tx1);
				if (flipv) std::swap(ty, ty1);
				// bottom left
//				vertices.insert(vertices.end(), {ox, oy, 0.0f, tx, ty1, 1, 1, 1, 1});
//				// bottom right
//				vertices.insert(vertices.end(), {ox + width_actual, oy, 0.0f, tx1, ty1, 1, 1, 1, 1});
//				// top right
//				vertices.insert(vertices.end(), {ox + width_actual, oy + height_actual, 0.0f, tx1, ty, 1, 1, 1, 1});
//				// top left
//				vertices.insert(vertices.end(), {ox, oy + height_actual, 0.0f, tx, ty, 1, 1, 1, 1});
//				unsigned ix = quadCount * 4;
//				indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
				//frameInfo.count += 6;
				quadCount++;
				// update static bounds
				m_modelBounds.min.x = std::min(m_modelBounds.min.x, ox);
				m_modelBounds.min.y = std::min(m_modelBounds.min.y, oy);
				m_modelBounds.max.x = std::max(m_modelBounds.max.x, ox + width_actual);
				m_modelBounds.max.y = std::max(m_modelBounds.max.y, oy + height_actual);

			}
			//auto itemId = item["id"].as<std::string>();
			//m_stateToItems[animId + "_" + std::to_string(frameCount)].push_back(m_sheet->getId(itemId));

			frameCount++;
			animInfo.frameCount++;
			animInfo.frameInfo.push_back(frameInfo);
		}
		m_animInfo.insert(std::make_pair(animId, animInfo));
	}
	//generateBuffers(vertices, indices);


}

void Sprite::init(Node * n) {
	auto obj = n->getUserData();
	obj["anim"] = m_defaultAnimation;
	obj["frame"] = 0;
}

Sprite::Sprite(SpriteBatch* batch, ShaderType type, GLenum primitive) : Model(type), _batch(batch) {
	m_primitive = primitive;
}


std::shared_ptr<Shape> Sprite::getRect(int mode, int x0, int x1, int y0, int y1) {
	if (mode == 0) {
		return std::make_shared<AABB>(x0, x1, y0, y1);
	} else {
		//return std::make_shared<AABB3D>(x0, x1, y0, y1, -m_halfThickness, m_halfThickness);
	}

}



const FrameInfo & Sprite::getFrameInfo(const std::string &anim, int frame) {
	return m_animInfo.at(anim).frameInfo[frame];
}

std::shared_ptr<Renderer> Sprite::getRenderer() const {
	return std::make_shared<SpriteRenderer>(_batch, m_defaultAnimation, m_texId, m_paletteId);

}



bool Sprite::hasCollision(const std::string & anim) const {
	return m_frameToShape.count(std::make_pair(anim, 0)) > 0;

}

std::shared_ptr<Shape> Sprite::getShape (const std::string& anim, int frame) const {
	auto it = m_frameToShape.find(std::make_pair(anim, frame));
	if (it == m_frameToShape.end())
		return nullptr;
	if (it->second == -1)
		return nullptr;
	return m_shapes[it->second];
}

std::shared_ptr<Shape> Sprite::getShapeCast (const std::string& anim, int frame) const {
	const auto& a = m_animInfo.at(anim).frameInfo[frame];
	if (a.attackBox == -1)
		return nullptr;
	return m_shapes[a.attackBox];
}

std::pair<int, int> Sprite::getDebugShape(const std::string &anim, int frame) {
	int boxId = m_animInfo.at(anim).frameInfo[frame].box;
	if (boxId == -1) {
		return std::make_pair(-1, -1);
	}
	return m_shapeInfo[boxId];
}

std::pair<int, int> Sprite::getDebugAttackShape(const std::string &anim, int frame) {
	int boxId = m_animInfo.at(anim).frameInfo[frame].attackBox;
	if (boxId == -1) {
		return std::make_pair(-1, -1);
	}
	return m_shapeInfo[boxId];
}


std::shared_ptr<Model> Sprite::generateDebugModel() {
	std::vector<float> vertices;
	std::vector<unsigned> elements;
	unsigned u{0};
	glm::vec4 color(1.f);
	auto model = std::make_shared<Model>(ShaderType::SHADER_COLOR, GL_LINES);
//
//    auto model = std::make_shared<AnimatedModel>(ShaderType::SHADER_COLOR, GL_LINES);
//
	// every shape is associated an offset and a count

	auto lambda = [&] (const Bounds& b) {
		vertices.insert(vertices.end(), {b.min.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
		vertices.insert(vertices.end(), {b.max.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
		vertices.insert(vertices.end(), {b.max.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
		vertices.insert(vertices.end(), {b.min.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
		elements.insert(elements.end(), {u, u+1, u+1, u+2, u+2, u+3, u+3, u});
		u += 4;
	};

	int ishape{0};
	for (const auto& s : m_shapes) {
		unsigned offset = elements.size();
		if (s->getShapeType() == ShapeType::COMPOUND) {
			auto* cs = static_cast<CompoundShape*>(s.get());
			for (const auto& t : cs->getShapes()) {
				lambda(t->getBounds());
			}
		} else {
			lambda(s->getBounds());
		}
		m_shapeInfo[ishape++] = std::make_pair(offset, elements.size() - offset);
//		model->addItem(offset, elements.size() - offset);
	}
	model->generateBuffers(vertices, elements);

	return model;
	//return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);

}