#include "font.h"
#include "util.h"
#include "assetmanager.h"
#include <fstream>
#include <iostream>

Font::Font(const std::string& fontId) {
    std::string filepath = "assets/" + fontId;

    std::ifstream infile(filepath);
    if (!infile.good()) {
        std::cout << "file not found: " << filepath;
        exit(1);
    }
    std::string textureFile;

    std::string line;

    std::getline(infile, textureFile);
    std::cout << " the tex filepath is " << textureFile << "\n";
    auto& am = AssetManager::instance();
    auto tex = am.getTex(textureFile);
    float tw = tex->getWidth();
    float th = tex->getHeight();
    //m_hasPalette = tex->hasPalette();
    _texId = tex->getTexId();

    std::getline(infile, line);
    _lineWidth = std::stof(line);

    while (std::getline(infile, line))
    {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        auto colon = line.find(':');
        std::string chars = line.substr(0, colon);
        auto data = strToVec<float>(line.substr(colon + 1));
        auto sss = getString32(chars);
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