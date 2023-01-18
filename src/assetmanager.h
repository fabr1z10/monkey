#pragma once

#include <unordered_map>
#include <memory>
#include "tex.h"
#include "palette.h"


class AssetManager {
private:
public:
    static AssetManager& instance() {
        static AssetManager instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    //std::shared_ptr<Sprite> getSprite(const std::string&);
    //std::shared_ptr<TiledModel> getTiled(const std::string&);
    std::shared_ptr<Tex> getTex(const std::string&);
    std::shared_ptr<Palette> getPalette(const std::string&);

private:

    std::unordered_map<std::string, std::shared_ptr<Tex>> m_tex;
    std::unordered_map<std::string, std::shared_ptr<Palette>> m_palettes;

};