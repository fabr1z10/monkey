//#include "tiled.h"
//#include "../engine.h"
//#include "../pyhelper.h"
//#include "../assetmanager.h"
//#include "../components/anim_tiled_renderer.h"
//#include <stack>
//
//TiledModel::TiledModel() : Model(ShaderType::SHADER_TEXTURE), m_i0(0) {
//
//}
//
//TiledModel::TiledModel(const pybind11::kwargs& args) : Model(ShaderType::SHADER_TEXTURE), m_i0(0) {
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
