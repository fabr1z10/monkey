#include "iquad.h"
#include "../components/quad_renderer.h"
#include "../pyhelper.h"
#include "../spritesheet.h"
#include "../math/random.h"
#include "../assetmanager.h"

using namespace pybind11::literals;

IQuads::IQuads(const std::string &batchId) : Model(), _batchId(batchId) {



    auto* batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(batchId));

    _sheet = batch->getSheet();
	auto * tex = _sheet->getTex();
	_texWidth = tex->getWidth();
	_texHeight = tex->getHeight();


}


std::string IQuads::ltrim(const std::string &s)
{
    static const std::string WHITESPACE = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

IQuads::IQuads(const std::string &batchId, const std::string &desc) : IQuads(batchId) {
    std::cout << "a\n";
    auto tileSize = AssetManager::instance().getSpritesheet(batchId)->getTileSize();
    auto tex = AssetManager::instance().getSpritesheet(batchId)->getTex();
    float cellSizeX = tex->getWidth() / tileSize.x;
    float cellSizeY = tex->getHeight() / tileSize.y;
    std::cout << tileSize.x << "\n";

    // stringstream class check1
    std::stringstream check1(desc);
    std::string intermediate;
    std::vector<std::string> data;
    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ','))
    {
        data.push_back(intermediate);
    }

    int x = 0;
    int y = 0;
    int i = 0;
    int pal =0;
    bool fliph = false;
    bool flipv = false;

    std::list<glm::ivec2> repeat;
    const std::string WHITESPACE = " \n\r\t\f\v";
    while (i < data.size()) {
        auto cmd = ltrim(data[i]);
        if (cmd == "jmp") {
            x = std::stoi(data[i+1]) * tileSize.x;
            y = std::stoi(data[i+2]) * tileSize.y;
            i += 3;
            continue;
        }
        if (cmd == "b") {
            x = 0;
            y += tileSize.y;
            i +=1;
            continue;
        }
        if (cmd == "rep") {
            // repeat
            int n = std::stoi(data[i+1]);
            repeat.push_front(glm::ivec2(n, i+2));
            i += 2;
            continue;
        }
        if (cmd == "loop") {
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
        if (data[i] == "fh") {
            fliph = true;
            i++;
            continue;
        }
        // flip next tile vertically
        if (data[i] == "fv") {
            flipv = true;
            i++;
            continue;
        }
        if (data[i] == "pal") {
            pal = std::stoi(data[i+1]);
            i+=2;
            continue;
        }

        try {
            int col = std::stoi(data[i]);
            if (col == -1) {
                i ++;
            }
            else {
                int row = std::stoi(data[i + 1]);
                float tx0 = col * tileSize.x;
                float ty0 = row * tileSize.y;
                auto textureCoordinates = glm::vec4(tx0, ty0, tileSize.x, tileSize.y);
                addQuad(textureCoordinates,
                        py::dict("pos"_a = glm::vec3(x, y, 0.f), "pal"_a = pal, "fliph"_a = fliph, "flipv"_a = flipv));
                i += 2;
            }
            x += tileSize.x;

        } catch (const std::invalid_argument& exc) {
            std::cerr << " EROR!\n";
            exit(1);
        }
        fliph = false;
        flipv = false;


    }
}

void IQuads::addQuad(glm::vec4 texCoords, const pybind11::kwargs &kwargs) {

	QuadInfo info;
	bool normalized = py_get_dict<bool>(kwargs, "normalized", false);
	glm::vec4 tc;
	if (!normalized) {
		float tx0 = texCoords.x / _texWidth;
		float tx1 = (texCoords.x + texCoords.z) / _texWidth;
		float ty0 = texCoords.y / _texHeight;
		float ty1 = (texCoords.y + texCoords.w) / _texHeight;
		tc = glm::vec4(tx0, tx1, ty0, ty1);
	} else {
		tc = glm::vec4(texCoords.x, texCoords.x + texCoords.z, texCoords.y, texCoords.y + texCoords.w);
	}

	info.textureCoordinates = tc;
	info.repeat = py_get_dict<glm::vec2>(kwargs, "repeat", glm::vec2(1.f));
	info.size = py_get_dict<glm::vec2>(kwargs, "size", glm::vec2(texCoords[2], texCoords[3]));
	info.location = py_get_dict<glm::vec3>(kwargs, "pos", glm::vec3());
	info.palette = py_get_dict<int>(kwargs, "pal", 0);
	info.fliph = py_get_dict<bool>(kwargs, "fliph", false);
	info.flipv = py_get_dict<bool>(kwargs, "flipv", false);
	_quads.push_back(info);
}


IQuadsRenderer::IQuadsRenderer(const std::string& batchId) : BatchRenderer<QuadBatch>(batchId) {
	auto tex = _batch->getSheet()->getTex();

}

void IQuadsRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	_model = std::dynamic_pointer_cast<IQuads>(model).get();
	auto qc = _model->getQuadCount();
	for (int i = 0; i< qc; ++i) {
		_primitiveIds.push_back(_batch->getPrimitiveId());
	}
}

void IQuadsRenderer::update(double) {
	auto worldTransform = m_node->getWorldMatrix();
	auto tex = _batch->getSheet()->getTex();
	auto tw = tex->getWidth();
	auto th = tex->getHeight();
	const auto& q = _model->getQuads();
	for (size_t i = 0; i < q.size(); ++i) {
		glm::vec3 bottomLeft = worldTransform * glm::vec4(q[i].location.x, q[i].location.y, q[i].location.z,1.f);
		// TODO THis calculation shouldbe done in addQuad!!
		//float tx0 = q[i].textureCoordinates[0] / tw;
		//float tx1 = (q[i].textureCoordinates[0] + q[i].textureCoordinates[2]) / tw;
		//float ty0 = q[i].textureCoordinates[1] / th;
		//float ty1 = (q[i].textureCoordinates[1] + q[i].textureCoordinates[3]) / th;
		_batch->setQuad(_primitiveIds[i],
						bottomLeft,
						q[i].size,
						q[i].textureCoordinates,
						q[i].repeat,
						q[i].palette,
						q[i].fliph,
						q[i].flipv,
						0.f,1.f,worldTransform);

	}
}

int Frame::getTicks() const {
	if (tickMin == -1) {
		return ticks;
	}
	return Random::instance().getUniform(tickMin, tickMin + maxTicks);

}

std::shared_ptr<Renderer> IQuads::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<IQuadsRenderer>(_batchId);

}



//StaticQuad::StaticQuad(const pybind11::kwargs& args) {
//	_textureCoordinates = py_get_dict<glm::vec4>(args, "tex_coords");
//	_size = py_get_dict<glm::vec2>(args, "size", glm::vec2(_textureCoordinates[2], _textureCoordinates[3]));
//	_anchorPoint = py_get_dict<glm::vec2>(args, "anchor", glm::vec2(0.f));
//	_repeat = py_get_dict<glm::vec2>(args, "repeat", glm::vec2(1.f, 1.f));
//}
//
//std::shared_ptr<Renderer> StaticQuad::getRenderer(const pybind11::kwargs & args) {
//	return std::make_shared<StaticQuadRenderer>(args);
//}