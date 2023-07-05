#include "assetmanager.h"
#include <iostream>

std::shared_ptr<Tex> AssetManager::getTex(const std::string & id) {
    std::string file = "assets/" + id;
    auto it = m_tex.find(file);
    if (it == m_tex.end()) {
        auto tex = std::make_shared<Tex>(file);
        m_tex[file] = tex;
        return tex;
    }
    return it->second;
}

std::shared_ptr<Sprite> AssetManager::getSprite(const std::string & id) {
	auto it = m_sprites.find(id);
	if (it == m_sprites.end()) {
		std::cout << " --- not cached. Create new!\n";
		auto u = id.find_last_of('/');
		auto spriteName = id.substr(u + 1);
		auto sheetName = id.substr(0, u);
		std::string file = "assets/" + sheetName + ".yaml";
		auto f = YAML::LoadFile(file);
		// check if spritesheet has been loaded
//        if (m_spriteSheets.count(sheetName) == 0) {
//        	// load spritesheet
//        	m_spriteSheets.insert(std::make_pair(sheetName, std::make_shared<SpriteSheet>(f)));
//        }
//		auto sheet = m_spriteSheets.at(sheetName);
		//auto sheetFile = f["sheet"].as<std::string>();
		auto spritesNode = f["sprites"];
		auto globals = f["globals"];

		for(YAML::const_iterator it=spritesNode.begin();it!=spritesNode.end();++it) {
			auto currId = it->first.as<std::string>();
			std::string cspr = id.substr(0, u+1) + currId;
			std::cout << " --- adding sprite: " << cspr << "\n";
			auto sprite = std::make_shared<Sprite>(it->second, globals);
			//sprite->initFromYAML(it->second, globals);
			m_sprites[cspr] = sprite;
		}
		if (m_sprites.count(id) == 0) {
			std::cerr << " looks like sprite: " << id << " does not exist!" << std::endl;
			exit(1);
		}
		return m_sprites.at(id);
	} else {
		return it->second;
	}
}

std::pair<std::string, std::string> AssetManager::splitFileAsset(const std::string & id) {
	auto u = id.find_last_of('/');
	auto assetName = id.substr(u + 1);
	auto prefix = id.substr(0, u);
	//std::string file = "assets/" + fileName + ".yaml";
	return std::make_pair(assetName, prefix);
}


std::shared_ptr<Palette> AssetManager::getPalette(const std::string& id) {
    auto it = m_palettes.find(id);
    if (it == m_palettes.end()) {
        auto ids = splitFileAsset(id);
        std::string file = "assets/" + ids.second + ".yaml";

        auto f = YAML::LoadFile(file);
        for(YAML::const_iterator it=f.begin();it!=f.end();++it) {
            auto currId = it->first.as<std::string>();
            auto pal = it->second.as<std::vector<unsigned char>>();
            std::string cspr = ids.second + "/" + currId;
            std::cout << " --- adding sprite: " << cspr << "\n";
            m_palettes[cspr] = std::make_shared<Palette>(pal);
        }
        return m_palettes.at(id);
    } else {
        return it->second;
    }

}

std::shared_ptr<Font> AssetManager::getFont(const std::string & id, const std::string& file) {
    auto it = m_fonts.find(id);
    if (it == m_fonts.end()) {
        std::cout << " --- not cached. Create new!\n";
        auto font = std::make_shared<Font>(file);
        m_fonts[id] = font;
        return font;
    }
    return it->second;
}