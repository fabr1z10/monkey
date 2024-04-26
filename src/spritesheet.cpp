#include "spritesheet.h"
#include <iostream>
#include "globals.h"
#include "error.h"
#include <cstdlib>
#include <iostream>
#include "yamlexp.h"
#include <filesystem>

namespace fs = std::filesystem;

std::string SpriteSheet::getTiled(const std::string & a) { //}, const pybind11::kwargs &b ) {
    return _tiled.at(a);
    //std::unordered_map<std::pair<std::string, const pybind11::kwargs&>, int> ciao;
    //ciao[std::make_pair(a, b)] = 20;

    //pybind11::eval()
}


SpriteSheet::SpriteSheet(const std::string& id, const std::string& fileName) : _id(id) {
	std::string directory = fileName.substr(0, fileName.rfind('/'));
	fs::path path = fileName;
	// this is the default file, if not specified in the YAML
	auto defaultPng = path.filename().replace_extension(".png");
	// check if file exists
	std::cout << " -- check if " << fileName << "exists.\n";
	YAML::Node f;
	try {
		f = YAML::LoadFile(fileName);
	} catch (YAML::BadFile& exc) {
		f = YAML::Node();
	}
	try {
		auto extIndex = fileName.find(".yaml");
		auto slashIndex = fileName.find_last_of('/');
		auto defaultSheetName = fileName.substr(slashIndex) + ".png";
		std::cout << "default sheet: " << defaultSheetName;
		auto sheet = f["sheet"].as<std::string>(defaultPng);

		_tileSize = f["tile_size"].as<glm::ivec2>(glm::ivec2(1, 1));
		_texture = std::make_shared<Tex>(directory + "/" + sheet);

		if (f["multi_sprites"]) {
			_multiNodes = f["multi_sprites"];
		}

		if (_texture->hasPalette()) {
			std::vector<unsigned char> colors;
			if (f["palettes"]) {
				std::cout << "texture is indexed. Loading additional palette ...\n";
				int numberOfExtraPals = f["palettes"].size();
				std::cout << "# of palettes: " << numberOfExtraPals << "\n";
				colors.resize(PALETTE_SIZE * (numberOfExtraPals + 1), 0);
				memcpy(&colors[0], _texture->getPal(), PALETTE_SIZE);

				size_t palOffset{0};
				for (const auto& pal : f["palettes"]) {
					auto palId = pal.first.as<std::string>();
					//std::cout << palId << "\n";
					palOffset += PALETTE_SIZE;
					for (const auto& color : pal.second) {
						auto key = color.first.as<std::string>();
						int colorIndex{0};

						if (key == "0") {
                            unsigned long colorHex = strtoul(key.c_str(), NULL, 16);
                            colorIndex = 0;
						} else {
							unsigned long colorHex = strtoul(key.c_str(), NULL, 16);
							colorIndex = _texture->getColorIndex(colorHex);
						}
						unsigned long destColor = strtoul(color.second.as<std::string>().c_str(), NULL, 16);
						//auto c = color.second.as<glm::vec3>();
						size_t i = palOffset + colorIndex * 4;
						colors[i++] = (destColor & 0xff0000) >> 16;
						colors[i++] = (destColor & 0x00ff00) >> 8;
						colors[i++] = destColor & 0x0000ff;
						colors[i] = 255;//(colorIndex == 0 ? 0 : 255);

					}
				}
			} else {
				colors.resize(PALETTE_SIZE);
				memcpy(&colors[0], _texture->getPal(), PALETTE_SIZE);
			}
			auto palette = std::make_shared<Palette>(colors);
			_texture->assignPalette(palette);
			//    m_defaultPaletteId = _pal->getTexId();
		}

		if (f["fonts"]) {
			for (const auto& f : f["fonts"]) {
				auto fontId = f.first.as<std::string>();
				_fonts[fontId] = std::make_shared<Font>(this, f.second);
			}
		}

		if (f["art"]) {
		    for (const auto& f : f["art"]) {
		        _tiled[f.first.as<std::string>()] = f.second.as<std::string>();
		    }
		}



		if (f["sprites"]) {
			for (const auto& sp : f["sprites"]) {
				auto spriteId = sp.first.as<std::string>();
				//std::cout << " reading sprite model: " << spriteId << std::endl;
				auto sprite = std::make_shared<Sprite>(this, sp.second);
				_sprites[spriteId] = sprite;
			}
		}

		// now loop through multi-sprites
//		if (f["multi_sprites"]) {
//			for (const auto& ms : f["multi_sprites"]) {
//				auto ms_id = ms.first.as<std::string>();
//				//std::cout << " reading multisprite: " << id << std::endl;
//				//std::shared_ptr<MultiNode> root;
//				_multiSprites[ms_id] = std::make_shared<MultiNode>(ms.second, _id);
//			}
//		}




	} catch (YAML::ParserException& exc) {
		GLIB_FAIL("Malformed file " << fileName << ": " << exc.what());
	}
//	for (const auto& ms : _sprites) {
//		std::cout << "sprite: " << ms.first << "\n";
//	}
//	for (const auto& ms : _multiSprites) {
//		std::cout << "multisprite: " << ms.first << "\n";
//	}


}

//std::string SpriteSheet::toString() {
////	std::stringstream str;
////	str << "sheet: " << _id << ", fonts: " << _fonts.size() << ", sprites: " << _sprites.size() << ", multisprites: " << _multiSprites.size() << std::endl;
////	return str.str();
//
//}

//std::shared_ptr<QuadBatch> SpriteSheet::getBatch() {
////	if (_batch == nullptr) {
////		_batch = std::make_shared<QuadBatch>(10000, this);
////	}
////
////	return _batch;
//
//}

std::shared_ptr<Sprite> SpriteSheet::getSprite(const std::string& id) {
	auto iter = _sprites.find(id);
	if (iter == _sprites.end()) {
		return nullptr;
	}
	return iter->second;
}

std::shared_ptr<MultiSprite> SpriteSheet::getMulti(const std::string & id) {
    return nullptr;
//	auto cino = _multiNodes[id];
//	if (cino) {
//		std::cout << "multi model " << id << " found.\n";
//		auto model = std::make_shared<MultiSprite>();
//		for (const auto& sprite : cino["sprites"]) {
//			model->addSprite(_sprites.at(sprite.as<std::string>()));
//		}
//		for (const auto& anim : cino["animations"]) {
//			auto animId = anim.first.as<std::string>();
//			model->addAnimation(animId);
//			int spriteId{0};
//			for (const auto& b : anim.second) {
//			    auto subAnim = b["anim"].as<std::string>();
//                auto offset = b["pos"].as<glm::vec3>(glm::vec3(0.f));
//                model->setAnimationData(animId, spriteId, subAnim, offset);
//                spriteId++;
//			}
//		}
//		return model;
//	} else {
//		return nullptr;
//	}

}

//std::shared_ptr<MultiNode> SpriteSheet::getMultiNode(const std::string& id, const std::string& batch) {
////	auto node = std::dynamic_pointer_cast<MultiNode>(_multiSprites.at(id)->clone());
////	node->initialize(batch);
////	return node;
//}
//
std::shared_ptr<Font> SpriteSheet::getFont(const std::string& id) {
	return _fonts.at(id);
}
