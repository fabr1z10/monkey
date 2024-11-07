#include <regex>
#include "tileparser.h"
#include "error.h"
#include "models/iquad.h"

using namespace pybind11::literals;

TileLanguageParser::TileLanguageParser(const std::string& batchId) : _batchId(batchId) {

	auto* batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(batchId));
	_sheet = batch->getSheet();
	_tileSize = _sheet->getTileSize();

	_funcs["q"] = &TileLanguageParser::quad;
	_funcs["go"] = &TileLanguageParser::go;
	_funcs["pal"] = &TileLanguageParser::pal;

	for (const auto& f : _funcs){
		_tokens.addToken(f.first);
	}
}

std::shared_ptr<Model> TileLanguageParser::createModel(const std::string & desc) {

	auto tokens = tokenize(desc, ";\n");

	_x = 0;
	_y = 0;
	_pal = 0;

	_model = std::make_shared<IQuads>(_batchId);
	for (auto& token : tokens) {
		// Remove all spaces and tabs
		token.erase(std::remove_if(token.begin(), token.end(), [](unsigned char c) {return c == ' ' || c == '\t';}), token.end());
		// Convert to lowercase using std::transform
		std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) { return std::tolower(c); });

		auto t = _tokens.findToken(token);
		if (t.empty()) {
			GLIB_FAIL("Cannot parse: " << token);
			exit(1);
		}
		//std::cout << " -- found token: " << t << "\n";
		(this->*_funcs[t])(token.substr(t.size()));
	}
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

void TileLanguageParser::quad(const std::string &args) {
	auto a = tokenize(args, ",");

	int x = std::stoi(a[0]) * _tileSize[0];
	int y = std::stoi(a[1]) * _tileSize[1];
	int w = std::stoi(a[2]) * _tileSize[0];
	int h = std::stoi(a[3]) * _tileSize[1];

	int rx = (a.size() > 4) ? std::stoi(a[4]) : 1;
	int ry = (a.size() > 5) ? std::stoi(a[5]) : 1;

	float totalWidth = rx * w;
	float totalHeight = ry * h;
	_model->addQuad( glm::vec4(x, y, w, h), py::dict(
			"pal"_a = _pal,
			"pos"_a = glm::vec3(_x,_y, 0),
			"repeat"_a = glm::vec2(rx, ry),
			"size"_a = glm::vec2(totalWidth, totalHeight)));

}

void TileLanguageParser::go(const std::string &args) {
	auto a = tokenize(args, ",");
	_x = std::stoi(a[0]) * _tileSize[0];
	_y = std::stoi(a[1]) * _tileSize[1];



}

void TileLanguageParser::pal(const std::string &args) {
	auto a = tokenize(args, ",");
	_pal = std::stoi(a[0]);




}