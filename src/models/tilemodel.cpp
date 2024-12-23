#include "tilemodel.h"
#include "../spritesheet.h"

TileModel::TileModel(const std::string& batchId, int width, int height, int frames, int ticksPerFrame, int scale) :
    _batchId(batchId), _width(width), _height(height), _quadCount(width * height), _tickPerFrames(ticksPerFrame), _frames(frames), _scale(scale) {
    auto* batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(batchId));
    auto sheet = batch->getSheet();

    _tileSize = sheet->getTileSize();

    _tilesInWidth = sheet->getTex()->getWidth() / _tileSize[0];
    _tilesInHeight = sheet->getTex()->getHeight() / _tileSize[1];
    _tileSize *= _scale;
    _frameInfo = std::vector<int>(width * height * frames, -1);
}

void TileModel::setTile(int pos, int tick, int tileId) {
    _frameInfo[_quadCount*tick + pos] = tileId;
}

int TileModel::addTile(int tileNumber, int pal, bool fliph, bool flipv) {
    int tx = tileNumber % _tilesInWidth;
    int ty = static_cast<int>(tileNumber / _tilesInHeight);
    float tx0 = static_cast<float>(tx) / _tilesInWidth;
    float ty0 = static_cast<float>(ty) / _tilesInHeight;
    glm::vec4 texCoords{tx0, tx0 + static_cast<float>(_scale) / _tilesInWidth, ty0, ty0 + static_cast<float>(_scale) / _tilesInHeight};
    TileInfo t(texCoords, pal, fliph, flipv);
    int n= _tiles.size();
    _tiles.push_back(t);
    return n;
}

const TileInfo* TileModel::getTile(int pos, int tick) {

    int c = _frameInfo[_quadCount * tick + pos];
    if (c == -1) {
        return nullptr;
    }
    return &_tiles[c];
}

std::shared_ptr<Renderer> TileModel::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<TileModelRenderer>(_batchId, _scale);

}

std::type_index TileModelRenderer::getType() {
    return std::type_index(typeid(Renderer));
}


TileModelRenderer::TileModelRenderer(const std::string& batchId, float scale) :
    BatchRenderer<QuadBatch>(batchId, pybind11::kwargs()), _tick(0), _frame(0) {
    _tileSize = _batch->getSheet()->getTileSize();
    _tileSize *= scale;
}

void TileModelRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
    Renderer::setModel(model, args);
    _model = std::dynamic_pointer_cast<TileModel>(model).get();
    _quadCount = _model->getQuadCount();
    for (int i = 0; i < _quadCount; ++i) {
        _primitiveIds.push_back(_batch->getPrimitiveId());
    }
    _frames = _model->getFrames();
    _ticksPerFrame = _model->getTicksPerFrame();
    _width = _model->getWidth();
}

void TileModelRenderer::update(double) {
    auto worldTransform = m_rendererTransform * m_node->getWorldMatrix();//m_node->getWorldMatrix();
    auto tex = _batch->getSheet()->getTex();
    auto tw = tex->getWidth();
    auto th = tex->getHeight();
    glm::vec3 bottomLeft = worldTransform[3];
    float x{0.f};
    float y{0.f};
    int col{0};
    for (size_t i = 0; i < _quadCount; ++i) {
        const auto* t = _model->getTile(i, _frame);
        if (t == nullptr) {
            _batch->setVisible(_primitiveIds[i], false);
        } else {
            _batch->setQuad(_primitiveIds[i],
                            bottomLeft + glm::vec3(x, y, 0.f),
                            _tileSize,
                            t->textureCoordinates,
                            glm::ivec2(1, 1),
                            t->palette,
                            t->fliph,
                            t->flipv,
                            0.f, glm::mat4(1.f));
        }
        x += 8.f;
        col += 1;
        if (col >= _width) {
            x = 0.f;
            y += 8.f;
            col = 0;
        }
    }
    _tick++;
    if (_tick >= _ticksPerFrame) {
        _tick = 0;
        _frame ++;
        if (_frame >= _frames) {
            _frame = 0;
        }
    }
}


