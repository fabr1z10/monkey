#pragma once

#include "trie.h"
#include "models/iquad.h"
#include "spritesheet.h"

class TileLanguageParser {
public:
	TileLanguageParser(const std::string& batchId);

	std::shared_ptr<Model> createModel(const std::string&);

	void import(const std::string& filename);

private:
	std::vector<std::string> tokenize(const std::string& s, const std::string& delimiters);
	void quad(const std::string& args);
	using FPtr = void (TileLanguageParser::*)(const std::string& args);
	Trie _tokens;
	std::unordered_map<std::string, FPtr> _funcs;
	SpriteSheet* _sheet;
	std::string _batchId;
	glm::ivec2 _tileSize;
	std::shared_ptr<IQuads> _model;


};
