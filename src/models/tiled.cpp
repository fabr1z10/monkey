#include "tiled.h"
//#include "../engine.h"
#include "../pyhelper.h"
#include "../assetmanager.h"
#include "../spritesheet.h"
#include <pybind11/stl.h>


//#include "../components/anim_tiled_renderer.h"
//#include <stack>
//
//TiledModel::TiledModel() : Model(ShaderType::SHADER_TEXTURE), m_i0(0) {
//
//}
//

std::string TiledModel::ltrim(const std::string &s)
{
	static const std::string WHITESPACE = " \n\r\t\f\v";
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}


int TiledModel::readFrame(const pybind11::handle & el, std::vector<Desc>& desc) {

	// Vector of string to save tokens
	std::vector<std::string> data;

	// stringstream class check1
	std::stringstream check1(el.cast<std::string>());

	std::string intermediate;

	// Tokenizing w.r.t. space ' '
	while(getline(check1, intermediate, ','))
	{
		data.push_back(intermediate);
	}

	int x = 0;
	int y = 0;
	int i = 0;

	std::list<glm::ivec2> repeat;
	const std::string WHITESPACE = " \n\r\t\f\v";
 	while (i < data.size()) {
		auto cmd = ltrim(data[i]);
		if (cmd == "JMP") {
			x = std::stoi(data[i+1]) * _tileSize.x;
			y = std::stoi(data[i+2]) * _tileSize.y;
			i += 3;
			continue;
		}
		if (cmd == "REP") {
			// repeat
			int n = std::stoi(data[i+1]);
			repeat.push_front(glm::ivec2(n, i+2));
			i += 2;
			continue;
		}
 		if (cmd == "LOOP") {
			if (repeat.empty()) {
				throw;
			}
			repeat.front()[0]--;
			if (repeat.front()[0] == 0) {
				repeat.pop_front();
				i += 1;
			} else {
				i = repeat.front()[1];
			}
			continue;
		}
		if (cmd == "TILE") {
			// sequence of tiles
			i += 1;
			bool fliph = false, flipv = false;
			int palette = 0;
			while (i < data.size()) {
				if (data[i] == ";") {
					i++;
					break;
				}
				// flip next tile horizontally
				if (data[i] == "FH") {
					fliph = true;
					i++;
				}
				// flip next tile vertically
				if (data[i] == "FV") {
					flipv = true;
					i++;
				}
				if (data[i] == "PAL") {
					palette = std::stoi(data[i+1]);
					i+=2;
				}
				Desc d;
				int ix = std::stoi(data[i]);
				int iy = std::stoi(data[i+1]);
				float tx0 = ix * _tsxNorm;
				float tx1 = (ix+1) * _tsxNorm;
				float ty0 = iy * _tsyNorm;
				float ty1 = (iy+1) * _tsyNorm;
				if (fliph) std::swap(tx0, tx1);
				if (flipv) std::swap(ty0, ty1);
				d.textureCoordinates = glm::vec4(tx0, tx1, ty0, ty1);
				d.size = _tileSize;
				d.location = glm::vec3(x, y, 0.f);
				d.palette = palette;
				d.repeat = glm::vec2(1.f, 1.f);
				desc.push_back(d);
				// reset flip
				fliph = false;
				flipv = false;
				x += _tileSize[0];
				i += 2;
			}

			continue;
		}
		if (cmd== "QUAD") {
			// size of quad
			Desc d;
			int w = std::stoi(data[i+1]);
			int h = std::stoi(data[i+2]);
			d.size.x = w * _tileSize.x;
			d.size.y = h * _tileSize.y;
			// read pattern
			int psx = std::stoi(data[i+3]);
			int psy = std::stoi(data[i+4]);
			int pw = std::stoi(data[i+5]);
			int ph = std::stoi(data[i+6]);
			d.repeat = {w / static_cast<float>(pw), h / static_cast<float>(ph)};
			float tx0 = psx * _tsxNorm;
			float tx1 = (psx+pw) * _tsxNorm;
			float ty0 = psy * _tsyNorm;
			float ty1 = (psy+ph) * _tsyNorm;
			d.location = {x, y, 0.f};
			i += 7;
			while (data[i] != ";") {
				if (data[i] == "PAL") {
					d.palette = std::stoi(data[i+1]);
					i += 2;
					continue;
				}
				if (data[i] == "OFFSET") {
					d.texOffset.x = std::stoi(data[i+1]) / static_cast<float>(pw);
					d.texOffset.y = std::stoi(data[i+2]) / static_cast<float>(ph);
					i += 3;
					continue;
				}
				if (data[i] == "FH") {
					std::swap(tx0, tx1);
					i += 1;
					continue;
				}
			}
			i++;
			d.textureCoordinates = {tx0, tx1, ty0, ty1};
			desc.push_back(d);

			continue;
		}


	}




}

TiledModel::TiledModel(const pybind11::kwargs& args) : Model(), _quadCount(0) {
    auto sheetId = py_get_dict<std::string>(args, "sheet");
    auto sheet = AssetManager::instance().getSpritesheet(sheetId);
    auto * tex = sheet->getTex();
    _tileSize = sheet->getTileSize();
    float texw = tex->getWidth();
    float texh = tex->getHeight();
    _tsxNorm = _tileSize[0] / static_cast<float>(texw);
    _tsyNorm = _tileSize[1] / static_cast<float>(texh);
    // order can be bottom to top (0) or top to bottom (1)
}

StaticTiledModelRenderer::StaticTiledModelRenderer(const pybind11::kwargs& args) : BatchRenderer<QuadBatch>(args) {


}


AnimatedTiledModelRenderer::AnimatedTiledModelRenderer(const pybind11::kwargs& args) : BatchRenderer<QuadBatch>(args),
        _frame(0), _ticks(0) {


}

void StaticTiledModelRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	auto tiledModel =  std::dynamic_pointer_cast<StaticTiledModel>(model);
	auto qc = tiledModel->getQuadCount();
	for (int i = 0; i< qc; ++i) {
		_primitiveIds.push_back(_batch->getPrimitiveId());
	}

	auto worldTransform = m_node->getWorldMatrix();

	int qid {0};
	for (const auto& quad : tiledModel->getQuads()) {
		auto flipx = m_node->getFlipX();

		auto bottomLeft = worldTransform * glm::vec4(m_shift + quad.location - glm::vec3(quad.anchorPoint, 0.f), 1.f);

		auto size = quad.size;
		_batch->setQuad(_primitiveIds[qid++],
						bottomLeft,
						size,
						quad.textureCoordinates,
						quad.repeat,
						quad.palette,
						flipx,
						quad.flipv,
						_zLayer,
						quad.texOffset);
	}

}

void AnimatedTiledModelRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs &args) {
	Renderer::setModel(model, args);
	auto tiledModel =  std::dynamic_pointer_cast<AnimatedTiledModel>(model);
	_model = tiledModel.get();
	auto qc = tiledModel->getQuadCount();
	for (int i = 0; i< qc; ++i) {
		_primitiveIds.push_back(_batch->getPrimitiveId());
	}
}


StaticTiledModel::StaticTiledModel(const pybind11::kwargs & args) : TiledModel(args) {

	readFrame(args["data"], _quads);
	_quadCount = _quads.size();
}


std::shared_ptr<Renderer> StaticTiledModel::getRenderer(const pybind11::kwargs& args) {
	return std::make_shared<StaticTiledModelRenderer>(args);
}

std::shared_ptr<Renderer> AnimatedTiledModel::getRenderer(const pybind11::kwargs & args) {
	return std::make_shared<AnimatedTiledModelRenderer>(args);
}


AnimatedTiledModel::AnimatedTiledModel(const pybind11::kwargs & args) : TiledModel(args) {

	_quadCount = -1;
	for (const auto& frame : args["frames"]) {
		Frame f;
		f.ticks = py_get_dict<int>(frame, "ticks", 10);
		readFrame(frame["data"], f._quads);
		if (_quadCount == -1) {
			_quadCount = f._quads.size();
		} else {
			assert(_quadCount == f._quads.size());
		}
		_frames.push_back(f);
	}


}


void AnimatedTiledModelRenderer::update(double) {
	auto worldTransform = m_node->getWorldMatrix();
	int qid{0};
	const auto& frame = _model->getFrame(_frame);
	for (const auto& quad : frame._quads) {
		auto flipx = m_node->getFlipX();
		auto bottomLeft = worldTransform * glm::vec4(m_shift + quad.location - glm::vec3(quad.anchorPoint, 0.f), 1.f);

		auto size = quad.size;
		_batch->setQuad(_primitiveIds[qid++],
						bottomLeft,
						size,
						quad.textureCoordinates,
						quad.repeat,
						quad.palette,
						flipx,
						quad.flipv,
						_zLayer,
						quad.texOffset);
	}
	//const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);

	if (_ticks >= frame.ticks) {
		// increment frame. if this animation is
		_frame ++;

		if (_frame >= _model->getFrameCount()) {
			_frame = 0;

		}
		_ticks = 0;
	} else {
		// if it's not time to update frame, increment current frame length
		_ticks++;
	}
}

//    // Vector of string to save tokens
//    readSheet(args);
//    std::vector<GLfloat> vertices;
//    std::vector<unsigned> indices;
//
//    auto s = py_get_dict<std::string>(args, "desc");
//    readTiles(s, vertices, indices);
//
//    // generate buffers
//    this->generateBuffers(vertices, indices);
//
//}
//
//AnimatedTiledModel::AnimatedTiledModel(const pybind11::kwargs& args) : TiledModel() {
//	readSheet(args);
//	std::vector<GLfloat> vertices;
//	std::vector<unsigned> indices;
//
//	for (const auto& frame : args["frames"]) {
//		auto d = frame.cast<pybind11::tuple>();
//		auto s = py_get<std::string>(d[0]);
//		int ticks = py_get<int>(d[1]);
//		auto offset = indices.size();
//		readTiles(s, vertices, indices);
//		auto count = indices.size() - offset;
//		m_frameInfo.emplace_back(offset, count, ticks);
//	}
//	// generate buffers
//	this->generateBuffers(vertices, indices);
//
//}
//
//
//void TiledModel::readSheet(const pybind11::kwargs& args) {
//	auto inf = std::numeric_limits<float>::max();
//	m_modelBounds.min = glm::vec3(inf, inf, -100.f);
//	m_modelBounds.max = glm::vec3(-inf, -inf, 100.f);
//    auto sheetId = py_get_dict<std::string>(args, "sheet");
//    const auto& game = Engine::instance().getConfig();
//    auto tileSets = py_get<pybind11::dict>(game, "tilesets");
//    auto sheet = tileSets[sheetId.c_str()].cast<pybind11::dict>();
//    auto imgName = py_get_dict<std::string>(sheet, "img");
//    m_tileSize = py_get_dict<glm::vec2>(sheet, "tile_size");
//    auto& am = AssetManager::instance();
//    auto tex = am.getTex(imgName);
//    if (tex->hasPalette()) {
//        m_shaderType = ShaderType::SHADER_TEXTURE_PALETTE;
//        m_paletteId = tex->getDefaultPaletteId();
//        auto palette = py_get_dict<std::string>(args, "palette", "");
//        if (!palette.empty()) {
//            auto pal = am.getPalette(palette);
//            m_paletteId = pal->getTexId();
//        }
//    }
//    m_t1 = static_cast<float>(m_tileSize[0]) / tex->getWidth();
//    m_t2 = static_cast<float>(m_tileSize[1]) / tex->getHeight();
//    m_texId = tex->getTexId();
//
//}
//
//void TiledModel::readTiles(const std::string& inputString, std::vector<GLfloat>& vertices, std::vector<unsigned>& indices) {
//    std::string u(inputString);
//    std::transform(u.begin(), u.end(), u.begin(), ::toupper);
//	u.erase(std::remove_if(u.begin(), u.end(), ::isspace), u.end());
//    std::vector<std::string> tokens;
//    std::stringstream check1(u);
//    std::string intermediate;
//    // Tokenizing w.r.t. space ' '
//    while(getline(check1, intermediate, ','))
//    {
//        tokens.push_back(intermediate);
//    }
//    unsigned n = 0;
//    bool fliph = false;
//    bool flipv = false;
//    int width = -1;
//    int x0 = 0;
//    int y0 = 0;
//    int x = x0;
//    int y = y0;
//    std::stack<std::pair<int, int>> loopStack;
//    //unsigned i0 = vertices.size();
//    bool horizontalFlip = false;
//    bool vFlip = false;
//    while (n < tokens.size()) {
//
//        if (tokens[n][0] == 'W') {
//            width = std::stoi(tokens[n+1]);
//            n += 2;
//            continue;
//        }
//
//        if (tokens[n][0] == 'R') {
//            loopStack.push(std::make_pair(n+2, std::stoi(tokens[n+1])-1));
//            n += 2;
//            continue;
//        }
//        if (tokens[n][0] == 'E') {
//            auto& t = loopStack.top();
//            if (t.second > 0) {
//                t.second -= 1;
//                n = t.first;
//            } else {
//                loopStack.pop();
//                n++;
//            }
//            continue;
//        }
//        if (tokens[n][0] == 'U') {
//            x = x0;
//            y += 1;
//            n++;
//            continue;
//        }
//        if (tokens[n][0] == 'H') {
//            // flip horizontal next tile only
//            n++;
//            horizontalFlip = true;
//            continue;
//        }
//		if (tokens[n][0] == 'V') {
//			// flip vertical next tile only
//			n++;
//			vFlip = true;
//			continue;
//		}
//        int tx = std::stoi(tokens[n++]);
//        if (tx != -1) {
//            int ty = std::stoi(tokens[n++]);
//            float tx0 = tx * m_t1;
//            float tx1 = (tx + 1.f) * m_t1;
//            float ty0 = ty * m_t2;
//            float ty1 = (ty + 1.f) * m_t2;
//            if (horizontalFlip) {
//                std::swap(tx0, tx1);
//                horizontalFlip = false;
//            }
//            if (vFlip) {
//            	std::swap(ty0, ty1);
//            	vFlip = false;
//            }
//            float xm = x * m_tileSize[0];
//            float ym = y * m_tileSize[1];
//            float xM = xm + m_tileSize[0];
//			float yM = ym + m_tileSize[1];
//
//			// bottom left
//            vertices.insert(vertices.end(),{xm, ym, 0.0f, tx0, ty1, 1, 1, 1, 1});
//            // bottom right
//            vertices.insert(vertices.end(),{xM, ym, 0.0f, tx1, ty1, 1, 1, 1, 1});
//            // top right
//            vertices.insert(vertices.end(),{xM, yM, 0.0f, tx1, ty0, 1, 1, 1, 1});
//            // top left
//            vertices.insert(vertices.end(),{xm, yM, 0.0f, tx0, ty0, 1, 1, 1, 1});
//            indices.insert(indices.end(), {m_i0, m_i0 + 1, m_i0 + 2, m_i0 + 3, m_i0, m_i0 + 2});
//			m_modelBounds.min.x = std::min(m_modelBounds.min.x, xm);
//			m_modelBounds.min.y = std::min(m_modelBounds.min.y, ym);
//			m_modelBounds.max.x = std::max(m_modelBounds.max.x, xM);
//			m_modelBounds.max.y = std::max(m_modelBounds.max.y, yM);
//            m_i0 += 4;
//        }
//        x++;
//        if (width != -1) {
//            if (x >= width) {
//                x %= width;
//                y++;
//            }
//        }
//
//
//
//
//
//    }
//}
//
//
//std::shared_ptr<Renderer> AnimatedTiledModel::getRenderer() const {
//	return std::make_shared<AnimatedTiledModelRenderer>(m_texId, m_paletteId);
//
//}
