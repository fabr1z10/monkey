#pragma once

#include <unordered_map>
#include <vector>
//#include "spritesheet.h"
//#include <memory>
#include "tex.h"
//#include "models/sprite.h"
//#include "palette.h"
//#include "font.h"
//#include "multinode.h"
//#include "models/polymesh.h"
//#include "skeletal/animation.h"


class AssetManager {
private:
public:

    static AssetManager& instance() {
        static AssetManager instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }


	//std::shared_ptr<SpriteSheet> readSpritesheet (const std::string& directory);
    //std::shared_ptr<Sprite> getSprite(const std::string&);
//    std::shared_ptr<MultiNode> getMulti(const std::string&, const std::string& batch);
//    //std::shared_ptr<TiledModel> getTiled(const std::string&);
    std::shared_ptr<Tex> getTex(const std::string&);
//    std::shared_ptr<Palette> getPalette(const std::string&);
//    std::shared_ptr<Font> getFont(const std::string&);
//	std::shared_ptr<PolyMesh> getPolyMesh(const std::string&);
//    std::shared_ptr<monkey::skeletal::SkeletalAnimation> getSkeletalAnimation(const std::string&);
private:
    std::string fileExists(const std::string&) const;
	AssetManager();
	std::pair<std::string, std::string> splitFileAsset(const std::string&);
	std::vector<std::string> _assetDirectories;
    //std::shared_ptr<SpriteSheet> getSpritesheet (const std::string& id);

	std::unordered_map<std::string, std::shared_ptr<Tex>> _tex;
//    std::unordered_map<std::string, std::shared_ptr<Palette>> m_palettes;
//	std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
//    std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
//    std::unordered_map<std::string, std::shared_ptr<MultiNode>> m_multiSprites;
    //std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> _spritesheets;
//    std::unordered_map<std::string, std::string> _spriteSheetMoniker;
//	std::unordered_map<std::string, std::shared_ptr<PolyMesh>> m_polymesh;
//    std::unordered_map<std::string, std::shared_ptr<monkey::skeletal::SkeletalAnimation>> m_skeletalAnimations;
};