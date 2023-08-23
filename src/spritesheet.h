#pragma once

#include "tex.h"
#include "palette.h"
#include "models/sprite.h"
#include "multinode.h"
#include "font.h"

class SpriteSheet {

public:
	SpriteSheet(const std::string& id, const std::string &directory);
	Tex* getTex();
	std::shared_ptr<QuadBatch> getBatch();
	std::shared_ptr<Sprite> getSprite(const std::string&);
	std::shared_ptr<MultiNode>  getMultiNode(const std::string&, const std::string& batch);
	std::shared_ptr<Font> getFont(const std::string&);
	std::string toString();
	std::string getId() const;
private:
	std::shared_ptr<Tex> _texture;
	std::shared_ptr<Palette> _palette;
	std::unordered_map<std::string, std::shared_ptr<Sprite>> _sprites;
	std::unordered_map<std::string, std::shared_ptr<MultiNode>> _multiSprites;
	std::unordered_map<std::string, std::shared_ptr<Font>> _fonts;
	std::shared_ptr<QuadBatch> _batch;
	std::string _id;

};

inline std::string SpriteSheet::getId() const {
	return _id;
}

inline Tex* SpriteSheet::getTex() {
	return _texture.get();

}
