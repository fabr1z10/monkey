#include "font.h"
#include "util.h"
#include "assetmanager.h"
#include <fstream>
#include <iostream>
#include "engine.h"
#include "spritesheet.h"
#include "yamlexp.h"
#include "room.h"

QuadBatch * Font::getBatch() {
	return _sheet->getBatch().get();
}

Font::Font(SpriteSheet* s, const YAML::Node& node) : _sheet(s) {

//    std::string filepath = "assets/" + fontId;
//
//    std::ifstream infile(filepath);
//    if (!infile.good()) {
//        std::cout << "file not found: " << filepath;
//        exit(1);
//    }
//    std::string textureFile;
//
//    std::string line;
//
//    std::getline(infile, textureFile);
//    std::cout << " the tex filepath is " << textureFile << "\n";
//    auto& am = AssetManager::instance();

    //_batch = Engine::instance().getRoom()->addSpriteBatch(textureFile);
    auto tex = s->getTex();//am.getTex(_batch->getSheet());
    float tw = tex->getWidth();
    float th = tex->getHeight();
    //m_hasPalette = tex->hasPalette();
    //_texId = tex->getTexId();

    _lineHeight = YAML::read<float>(node, "line_height");

    for (const auto& n : node["chars"]) {
    	auto chars = n.first.as<std::string>();
		auto sss = getString32(chars);
        auto data = n.second.as<std::vector<float>>();
        int u = 0;
        for (char32_t w : sss) {
            if (u % 2 == 0) {
                m_info[w] = CharInfo{
                        data[0] / tw, data[1] / th, data[2] / tw, data[3] / th,     // tex coords
                        data[2], data[3],                                                    // size
                        data[4],                                                            // advance
                        data.size() > 5 ? data[5] : 0};                                         // offset y
            }
            u++;
        }
    }
}

const CharInfo & Font::getCharInfo(char32_t c) {
    return m_info.at(c);
}