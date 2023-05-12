#pragma once

#include <unordered_map>
#include <memory>
#include "tex.h"
#include "models/sprite.h"
#include "palette.h"


class AssetManager {
private:
public:
    static AssetManager& instance() {
        static AssetManager instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    std::shared_ptr<Sprite> getSprite(std::shared_ptr<IBatch> batch, const std::string&);
    //std::shared_ptr<TiledModel> getTiled(const std::string&);
    std::shared_ptr<Tex> getTex(const std::string&);
    std::shared_ptr<Palette> getPalette(const std::string&);

private:
	std::pair<std::string, std::string> splitFileAsset(const std::string&);

	std::unordered_map<std::string, std::shared_ptr<Tex>> m_tex;
    std::unordered_map<std::string, std::shared_ptr<Palette>> m_palettes;
	std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;

};