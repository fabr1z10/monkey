#pragma once

#include "trie.h"
#include <stack>
#include "models/iquad.h"
#include "spritesheet.h"

class TileLanguageParser {
public:
    static std::shared_ptr<TileLanguageParser> getTileParser(const std::string& sheetId);

	std::shared_ptr<Model> createModel(const std::string&);

	void import(const std::string& filename);

private:
    TileLanguageParser(const std::string& batchId);

    std::unordered_map<std::string, std::shared_ptr<Model>> _modelCache;
    struct MachineState {
        MachineState() : i(0), x(0), y(0), pal(0) {}
        // current instruction
        int i;
        // cursor position in tile coordinates
        int x;
        int y;
        int pal;
        std::stack<std::pair<int, int>> loop;
    };
	std::vector<std::string> tokenize(const std::string& s, const std::string& delimiters);
	void quad(const std::vector<std::string>& args, MachineState& s);
	void go(const std::vector<std::string>& args, MachineState& s);
	void pal(const std::vector<std::string>& args, MachineState& s);
	void rep(const std::vector<std::string>& args, MachineState& s);
    void loop(const std::vector<std::string>& args, MachineState& s);
    void tile(const std::vector<std::string>& args, MachineState& s);
    void up(const std::vector<std::string>& args, MachineState& s);
	int _x, _y, _pal;
	using FPtr = void (TileLanguageParser::*)(
	        const std::vector<std::string>& args,
	        MachineState& s);
	Trie _tokens;
	std::unordered_map<std::string, FPtr> _funcs;
	SpriteSheet* _sheet;
	std::string _batchId;
	glm::ivec2 _tileSize;
	int _widthInTiles;
	int _heightInTiles;
	std::shared_ptr<IQuads> _model;


};
