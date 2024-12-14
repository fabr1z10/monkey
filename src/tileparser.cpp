#include <regex>
#include "tileparser.h"
#include "error.h"
#include "models/iquad.h"

using namespace pybind11::literals;

std::shared_ptr<TileLanguageParser> TileLanguageParser::getTileParser(const std::string &sheetId) {
	static std::unordered_map<std::string, std::shared_ptr<TileLanguageParser>> parsers;
	auto id = parsers.find(sheetId);
	if (id == parsers.end()) {
		parsers[sheetId] = std::shared_ptr<TileLanguageParser>(new TileLanguageParser(sheetId));
		return parsers.at(sheetId);
	}
	return id->second;
}

TileLanguageParser::TileLanguageParser(const std::string& batchId) : _batchId(batchId) {

	auto* batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(batchId));
	_sheet = batch->getSheet();
	_tileSize = _sheet->getTileSize();
	_widthInTiles = _sheet->getTex()->getWidth() / _tileSize[0];
	_heightInTiles = _sheet->getTex()->getHeight() / _tileSize[1];

	_funcs["q"] = &TileLanguageParser::quad;
	_funcs["t"] = &TileLanguageParser::tile;
	_funcs["go"] = &TileLanguageParser::go;
	_funcs["pal"] = &TileLanguageParser::pal;
	_funcs["rep"] = &TileLanguageParser::rep;
	_funcs["loop"] = &TileLanguageParser::loop;
	_funcs["up"] = &TileLanguageParser::up;

	for (const auto& f : _funcs){
		_tokens.addToken(f.first);
	}
}

std::shared_ptr<Model> TileLanguageParser::createModel(const std::string & desc) {

	auto it = _modelCache.find(desc);
	if (it != _modelCache.end()) {
		std::cout << "cache hit!\n";
		return it->second;
	}

	auto tokens = tokenize(desc, ";\n");


	MachineState state;

	_model = std::make_shared<IQuads>(_batchId);

	std::stack<std::pair<int, int>> loop;

	while (state.i < tokens.size()) {
		std::string current(tokens[state.i]);
		// Remove all spaces and tabs
		current.erase(std::remove_if(current.begin(), current.end(),
							   [](unsigned char c) {return c == ' ' || c == '\t';}), current.end());
		// Convert to lowercase using std::transform
		std::transform(current.begin(), current.end(), current.begin(), [](unsigned char c) { return std::tolower(c); });

		auto t = _tokens.findToken(current);
		if (t.empty()) {
			GLIB_FAIL("Cannot parse: " << current);
		}

		auto args = tokenize(current.substr(t.size()), ",");
		(this->*_funcs[t])(args, state);
	}
	_modelCache[desc] = _model;
	return _model;
}

std::vector<std::string> TileLanguageParser::tokenize(const std::string &s, const std::string& delimiters) {
	std::regex regex("["+ delimiters + "]");
	std::sregex_token_iterator it(s.begin(), s.end(), regex, -1);
	std::sregex_token_iterator end;
	std::vector<std::string> tokens;
	// Collect tokens that are non-empty
	for (; it != end; ++it) {
		if (!it->str().empty()) {
			tokens.push_back(*it);
		}
	}
	return tokens;
}

void TileLanguageParser::tile(const std::vector<std::string>& a, MachineState& s) {
	for (const auto& tileId : a) {
		int tile;
		bool flipH{false};
		if (tileId[0] == 'h') {
			tile = std::stoi(tileId.substr(1));
			flipH = true;
		} else {
			tile = std::stoi(tileId);
		}
		if (tile == -1) {
			s.x++;
			continue;
		}
		float tx = _tileSize[0] * (tile % _widthInTiles);
		float ty = _tileSize[1] * (tile / _heightInTiles);
		float x = s.x * _tileSize[0];
		float y = s.y * _tileSize[1];
		_model->addQuad( glm::vec4(tx, ty, _tileSize[0], _tileSize[1]), py::dict(
				"pal"_a = s.pal,
				"pos"_a = glm::vec3(x, y, 0),
				"repeat"_a = glm::vec2(1, 1),
				"fliph"_a = flipH,
				"size"_a = glm::vec2(_tileSize[0], _tileSize[1])));
		s.x++;
	}
	s.i++;
}


void TileLanguageParser::quad(const std::vector<std::string>& a, MachineState& s) {
	int x = std::stoi(a[0]) * _tileSize[0];
	int y = std::stoi(a[1]) * _tileSize[1];
	int w = std::stoi(a[2]) * _tileSize[0];
	int h = std::stoi(a[3]) * _tileSize[1];

	int rx = (a.size() > 4) ? std::stoi(a[4]) : 1;
	int ry = (a.size() > 5) ? std::stoi(a[5]) : 1;

	float totalWidth = rx * w;
	float totalHeight = ry * h;
	float px = s.x * _tileSize[0];
	float py = s.y * _tileSize[0];
	_model->addQuad( glm::vec4(x, y, w, h), py::dict(
			"pal"_a = _pal,
			"pos"_a = glm::vec3(px, py, 0),
			"repeat"_a = glm::vec2(rx, ry),
			"size"_a = glm::vec2(totalWidth, totalHeight)));
	s.x += rx *  std::stoi(a[2]);
	s.i ++;
}

void TileLanguageParser::go(const std::vector<std::string>& a, MachineState& s) {
	s.x = std::stoi(a[0]);
	s.y = std::stoi(a[1]);
	s.i++;
}

void TileLanguageParser::pal(const std::vector<std::string>& a, MachineState& s) {
	s.pal = std::stoi(a[0]);
	s.i++;
}

void TileLanguageParser::rep(const std::vector<std::string> &args, MachineState &s) {
	s.loop.push({s.i+1, std::stoi(args[0])-1});
	s.i++;
}

void TileLanguageParser::up(const std::vector<std::string> &args, MachineState &s) {
	s.x = 0;
	s.y ++;
	s.i++;
}


void TileLanguageParser::loop(const std::vector<std::string> &args, MachineState &s) {
	if (s.loop.empty()) {
		std::cout << "Loop without repeat\n";
		exit(1);
	}
	auto& stackTop = s.loop.top();
	if (stackTop.second == 0) {
		s.loop.pop();
		s.i++;
	} else {
		stackTop.second--;
		s.i = stackTop.first;
	}

}