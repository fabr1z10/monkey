#include "assetmanager.h"


std::shared_ptr<Tex> AssetManager::getTex(const std::string & id) {
    std::string file = "assets/" + id;
    auto it = m_tex.find(file);
    if (it == m_tex.end()) {
        auto tex = std::make_shared<Tex>(file);
        m_tex[id] = tex;
        return tex;
    }
    return it->second;
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
//            auto pal = it->second.as<std::vector<unsigned>>();
//            std::string cspr = ids.second + "/" + currId;
//            std::cout << " --- adding sprite: " << cspr << "\n";
//            m_palettes[cspr] = std::make_shared<Palette>(pal);
//        }
//        return m_palettes.at(id);
//    } else {
//        return it->second;
//    }

}