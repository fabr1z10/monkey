#include "spritesheet.h"
#include <iostream>
#include "globals.h"
#include "error.h"
#include <cstdlib>
#include <iostream>
#include "yamlexp.h"

SpriteSheet::SpriteSheet(const std::string& id, const std::string &file) : _id(id) {
	std::string directory = file.substr(0, file.rfind('/'));
	std::string fileName = file + ".yaml";
	std::cout << fileName << "\n";


	try {
		auto f = YAML::LoadFile(fileName);
		if (f.IsDefined()) {
			std::cout << "Ok\n";
		} else {
			std::cout << "mmm\n";
		}

		auto sheet = f["sheet"].as<std::string>();
		_texture = std::make_shared<Tex>(directory + "/" + sheet);

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
					std::cout << palId << "\n";
					palOffset += PALETTE_SIZE;
					for (const auto& color : pal.second) {
						auto key = color.first.as<std::string>();
						int colorIndex{0};
						if (key == "0") {

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



		if (f["sprites"]) {
			for (const auto& sp : f["sprites"]) {
				auto spriteId = sp.first.as<std::string>();
				//std::cout << " reading sprite model: " << spriteId << std::endl;
				auto sprite = std::make_shared<Sprite>(this, sp.second);
				_sprites[spriteId] = sprite;
			}
		}

		// now loop through multi-sprites
		if (f["multi_sprites"]) {
			for (const auto& ms : f["multi_sprites"]) {
				auto ms_id = ms.first.as<std::string>();
				//std::cout << " reading multisprite: " << id << std::endl;
				//std::shared_ptr<MultiNode> root;
				_multiSprites[ms_id] = std::make_shared<MultiNode>(ms.second, id);
			}
		}




	} catch (YAML::BadFile& exc) {
		GLIB_FAIL("Error opening " << fileName << ": " << exc.what());
	} catch (YAML::ParserException& exc) {
		GLIB_FAIL("Malformed file " << fileName << ": " << exc.what());
	}
	for (const auto& ms : _sprites) {
		std::cout << "sprite: " << ms.first << "\n";
	}
	for (const auto& ms : _multiSprites) {
		std::cout << "multisprite: " << ms.first << "\n";
	}


}

std::string SpriteSheet::toString() {
	std::stringstream str;
	str << "sheet: " << _id << ", fonts: " << _fonts.size() << ", sprites: " << _sprites.size() << ", multisprites: " << _multiSprites.size() << std::endl;
	return str.str();

}

std::shared_ptr<QuadBatch> SpriteSheet::getBatch() {
//	if (_batch == nullptr) {
//		_batch = std::make_shared<QuadBatch>(10000, this);
//	}
//
//	return _batch;

}

std::shared_ptr<Sprite> SpriteSheet::getSprite(const std::string& id) {
	return _sprites.at(id);
}

std::shared_ptr<MultiNode> SpriteSheet::getMultiNode(const std::string& id, const std::string& batch) {
	auto node = std::dynamic_pointer_cast<MultiNode>(_multiSprites.at(id)->clone());
	node->initialize(batch);
	return node;
}

std::shared_ptr<Font> SpriteSheet::getFont(const std::string& id) {
	return _fonts.at(id);
}
