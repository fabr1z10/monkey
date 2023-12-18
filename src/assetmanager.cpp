#include "assetmanager.h"
#include <iostream>
#include "room.h"
#include "engine.h"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include "error.h"
#include "globals.h"
#include "yamlexp.h"
#include "spritesheet.h"
#include "pyhelper.h"


AssetManager::AssetManager() {

	auto settings = Engine::instance().getConfig();
	_assetDirectory = py_get<std::string>(settings, "asset_directory");
}


std::shared_ptr<Tex> AssetManager::getTex(const std::string & file) {
    auto it = m_tex.find(file);
    if (it == m_tex.end()) {
        auto tex = std::make_shared<Tex>(file);
        m_tex[file] = tex;
        return tex;
    }
    return it->second;
}


std::shared_ptr<SpriteSheet> AssetManager::getSpritesheet(const std::string &file) {
	std::cout << "ciaone\n";
	//	auto sheets = py_get<pybind11::dict>(m_settings, "spritesheets", pybind11::dict());
//	for (const auto& sheet : sheets) {
//
//		auto id = sheet.first.cast<std::string>();
//		auto file = sheet.second.cast<std::string>();
//		std::cout << "READING SPRITESHEET " << id << " AT " << file << "\n";
//		AssetManager::instance().readSpritesheet(id, file);
//
//	}
    auto it = _spriteSheetMoniker.find(file);
    if (it != _spriteSheetMoniker.end()) {
        return m_spritesheets.at(it->second);
    } else {
	    std::string filename = _assetDirectory + '/' + file;
        auto sheet = std::make_shared<SpriteSheet>(filename);
        _spriteSheetMoniker[file] = sheet->getId();
        m_spritesheets[sheet->getId()] = sheet;
        return sheet;
	}

}

//void AssetManager::readSpritesheet(const std::string &id, const std::string &fileName) {
//
//	auto sheet = std::make_shared<SpriteSheet>(fileName);
//	return sheet;
//		m_spritesheets[id] = sheet;
//		std::cout << sheet->toString();
//	}
//	//return m_spritesheets.at(id);
//
//}

std::shared_ptr<MultiNode> AssetManager::getMulti(const std::string & id, const std::string& batch) {
	auto u = id.find('/');
	auto sheet = id.substr(0, u);
	auto sprite = id.substr(u+1);
	return m_spritesheets.at(sheet)->getMultiNode(sprite, batch);

}

std::shared_ptr<Sprite> AssetManager::getSprite(const std::string & id) {

	auto u = id.find('/');
	auto sheet = id.substr(0, u);
	auto sprite = id.substr(u+1);

	return m_spritesheets.at(sheet)->getSprite(sprite);
}
//	auto it = m_sprites.find(id);
//	if (it == m_sprites.end()) {
//		std::cout << " --- not cached. Create new!\n";
//		auto u = id.find_last_of('/');
//		auto spriteName = id.substr(u + 1);
//		auto sheetName = id.substr(0, u);
//		std::string file = "assets/" + sheetName + ".yaml";
//		auto f = YAML::LoadFile(file);
//		// check if spritesheet has been loaded
////        if (m_spriteSheets.count(sheetName) == 0) {
////        	// load spritesheet
////        	m_spriteSheets.insert(std::make_pair(sheetName, std::make_shared<SpriteSheet>(f)));
////        }
////		auto sheet = m_spriteSheets.at(sheetName);
//		//auto sheetFile = f["sheet"].as<std::string>();
//		auto spritesNode = f["sprites"];
//		auto sheet = f["sheet"].as<std::string>();
//		auto * batch = Engine::instance().getRoom()->addSpriteBatch(sheet);
//
//
//		// create batch, - if not there already
//		auto globals = f["globals"];
//
//		for(YAML::const_iterator it=spritesNode.begin();it!=spritesNode.end();++it) {
//			auto currId = it->first.as<std::string>();
//			std::string cspr = id.substr(0, u+1) + currId;
//			std::cout << " --- adding sprite: " << cspr << "\n";
//			auto sprite = std::make_shared<Sprite>(batch, it->second);
//			//sprite->initFromYAML(it->second, globals);
//			m_sprites[cspr] = sprite;
//		}
//		if (m_sprites.count(id) == 0) {
//			std::cerr << " looks like sprite: " << id << " does not exist!" << std::endl;
//			exit(1);
//		}
//		return m_sprites.at(id);
//	} else {
//		return it->second;
//	}
//}

std::pair<std::string, std::string> AssetManager::splitFileAsset(const std::string & id) {
	auto u = id.find_last_of('/');
	auto assetName = id.substr(u + 1);
	auto prefix = id.substr(0, u);
	//std::string file = "assets/" + fileName + ".yaml";
	return std::make_pair(assetName, prefix);
}


std::shared_ptr<Palette> AssetManager::getPalette(const std::string& id) {
//    auto it = m_palettes.find(id);
//    if (it == m_palettes.end()) {
//        auto ids = splitFileAsset(id);
//        std::string file = "assets/" + ids.second + ".yaml";
//
//        auto f = YAML::LoadFile(file);
//        for(YAML::const_iterator it=f.begin();it!=f.end();++it) {
//            auto currId = it->first.as<std::string>();
//            auto pal = it->second.as<std::vector<unsigned char>>();
//            std::string cspr = ids.second + "/" + currId;
//            std::cout << " --- adding sprite: " << cspr << "\n";
//            m_palettes[cspr] = std::make_shared<Palette>(pal);
//        }
//        return m_palettes.at(id);
//    } else {
//        return it->second;
//    }

}

std::shared_ptr<Font> AssetManager::getFont(const std::string& id) {
	auto u = id.find('/');
	auto sheet = id.substr(0, u);
	auto fontName = id.substr(u+1);
	return m_spritesheets.at(sheet)->getFont(fontName);
//    auto it = m_fonts.find(file);
//    if (it == m_fonts.end()) {
//        std::cout << " --- not cached. Create new!\n";
//        auto font = std::make_shared<Font>(file);
//        m_fonts[file] = font;
//        return font;
//    }
//    return it->second;
}


std::shared_ptr<PolyMesh> AssetManager::getPolyMesh(const std::string& id) {
	auto it = m_polymesh.find(id);
	if (it == m_polymesh.end()) {
		std::cout << " --- not cached. Create new!\n";
		auto u = id.find_last_of('/');
		if (u == std::string::npos) {
			std::cerr << " --- wrong asset format. must have a /";
			exit(1);
		}
		auto meshName = id.substr(u + 1);
		auto sheetName = id.substr(0, u);
		std::string file = sheetName + ".yaml";
		auto f = YAML::LoadFile(file);
		for(YAML::const_iterator it2 = f.begin(); it2 != f.end(); ++it2) {
			auto currId = it2->first.as<std::string>();
			if (currId == meshName) {
				std::cout << " --- mesh: " << id << "\n";
				m_polymesh[id] = std::make_shared<PolyMesh>(it2->second);
			}
		}
		if (m_polymesh.count(id) == 0) {
			std::cerr << " looks like sprite: " << id << " does not exist!" << std::endl;
			exit(1);
		}
		return m_polymesh.at(id);
	} else {
		return it->second;
	}

}

std::shared_ptr<monkey::skeletal::SkeletalAnimation> AssetManager::getSkeletalAnimation(const std::string & id) {
    auto it = m_skeletalAnimations.find(id);
    if (it == m_skeletalAnimations.end()) {
        std::cout << " --- not cached. Create new!\n";
        auto u = id.find_last_of('/');
        if (u == std::string::npos) {
            std::cerr << " --- wrong asset format. must have a /";
            exit(1);
        }
        auto meshName = id.substr(u + 1);
        auto sheetName = id.substr(0, u);
        std::string file = _assetDirectory + "/" + sheetName + ".yaml";
        try {

            auto f = YAML::LoadFile(file);
            for (YAML::const_iterator it2 = f.begin(); it2 != f.end(); ++it2) {
                auto currId = it2->first.as<std::string>();
                if (currId == meshName) {
                    std::cout << " --- mesh: " << id << "\n";
                    m_skeletalAnimations[id] = std::make_shared<monkey::skeletal::SkeletalAnimation>(it2->second);
                }
            }
            if (m_skeletalAnimations.count(id) == 0) {
                GLIB_FAIL("Looks like sprite: " + id + " does not exist!")
            }
            return m_skeletalAnimations.at(id);
        } catch(const YAML::ParserException& ex) {
            GLIB_FAIL(ex.what())
        } catch (const YAML::BadFile& ex) {
            GLIB_FAIL("Error loading " << file << ": "<< ex.what())
            //std::cout << "\033[1;31mError loading " << file << ": " << ex.what() << std::endl;
            //exit(1);

        }
    } else {
        return it->second;
    }

}