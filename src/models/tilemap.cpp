#include "tilemap.h"
#include "../spritesheet.h"
#include "../engine.h"
#include "../pyhelper.h"

TileMap::TileMap(const pybind11::kwargs &args) : _layers(1) {
    _deviceSize = Engine::instance().getDeviceSize();
    _tileSize = py_get_dict<int>(args, "tile_size", 8);

}


std::shared_ptr<Renderer> TileMap::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<TileMapRenderer>(args);

}
void TileMap::setTile(int x, int y, int i, int j, glm::vec2 offset) {
    int nextId = _tiles.size();
    _tiles.push_back(Tile{i, j, glm::vec2(x, y)});
    placeOnGrid(nextId, x, y);
}

void TileMap::placeOnGrid(int id, float x, float y) {
    int x0 =static_cast<int>(x) / _tileSize;
    int xr =static_cast<int>(x) % _tileSize;
    int y0 =static_cast<int>(x) / _tileSize;
    int yr =static_cast<int>(x) % _tileSize;


    _grid[{x0, y0}].insert(id);
    _tiles[id].grid = {x0, y0, x0 + (xr > 0 ? 1: 0), y0 + (yr > 0 ? 1 : 0) };

    if (xr > 0) {
        _grid[{x0+1, y0}].insert(id);
    }
    if (yr > 0) {
        _grid[{x0, y0+1}].insert(id);
    }
    if (xr > 0 and yr > 0) {
        _grid[{x0+1, y0+1}].insert(id);
    }
}

void TileMap::move(int id, float x, float y) {
    if (_tiles[id].x == x && _tiles[id].y) return;
    // remove old entry
    _grid.at({_tiles[id].grid[0], _tiles[id].grid[1]}).erase(id);
    if (_tiles[id].grid[2] > _tiles[id].grid[0]) {
        _grid.at({_tiles[id].grid[2], _tiles[id].grid[1]}).erase(id);
    }
    if (_tiles[id].grid[3] > _tiles[id].grid[1]) {
        _grid.at({_tiles[id].grid[0], _tiles[id].grid[3]}).erase(id);
    }
    if ((_tiles[id].grid[2] > _tiles[id].grid[0]) && (_tiles[id].grid[3] > _tiles[id].grid[1])) {
        _grid.at({_tiles[id].grid[1], _tiles[id].grid[3]}).erase(id);
    }
    placeOnGrid(id, x, y);



}

const std::unordered_set<int>* TileMap::getTiles(int x, int y) {
    auto it = _grid.find(glm::ivec2(x, y));
    if (it == _grid.end()) {
        return nullptr;
    }
    return &it->second;

}

TileMapRenderer::TileMapRenderer(const pybind11::kwargs& args) : BatchRenderer<QuadBatch>(args) {
    //_batch = dynamic_cast<QuadBatch*>(batch);
    assert(_batch != nullptr);

}

void TileMapRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs &) {
    _tileMap = std::dynamic_pointer_cast<TileMap>(model).get();
    _tileSize = _tileMap->getTileSize();
    _viewArea = glm::vec2(0.f);
    _deviceSize = _tileMap->getDeviceSize();
    _columns = _deviceSize.x / _tileSize + 1;
    _rows = _deviceSize.y / _tileSize + 1;
    int totalTiles = _columns * _rows * _tileMap->getLayerCount();
    for (int i = 0; i < totalTiles; ++i) {
        _primitiveIds.push_back(_batch->getPrimitiveId());
    }
    _sheetSize = {_batch->getSheet()->getTex()->getWidth(), _batch->getSheet()->getTex()->getHeight()};
    _sheetSize.x /= _tileSize;
    _sheetSize.y /= _tileSize;
    _invSheetSize = {1.f / _sheetSize.x, 1.f / _sheetSize.y};
    _timer=0.0;
}

void TileMapRenderer::update(double dt) {
    _timer+=dt;
    glm::ivec2 itile = _viewArea / _tileSize;
    glm::vec2 bottomLeft = - (_viewArea - itile * _tileSize);

    //glm::vec3 bl(bottomLeft, 0.f);
    int k = 0;



    for (int i = itile.y; i < _rows; ++i) {
        for (int j = itile.x; j < _columns; ++j) {
            const auto* tiles =_tileMap->getTiles(j,i);
            if (tiles != nullptr) {
                for (const auto &tileId : *tiles) {
                    auto &tile = _tileMap->getTile(tileId);
                    glm::vec4 texCoordinates{tile.x * _invSheetSize.x, (tile.x + 1) * _invSheetSize.x,
                                             tile.y * _invSheetSize.y, (tile.y + 1) * _invSheetSize.y};
                    glm::vec3 bottomLeft{tile.position.x, tile.position.y, 0.f};
                    _batch->setQuad(k++, bottomLeft, glm::vec2(_tileSize, _tileSize), texCoordinates,
                                    glm::vec2(1.f, 1.f), 0,
                                    false, false, 0, 1.f);
                    tile.position.x = 10.f * sin(2.f * _timer);
                    tile.position.y = _tileSize * tileId;

                }
            }
            bottomLeft.x += _tileSize;
        }
        bottomLeft.x = 0;
        bottomLeft.y += _tileSize;

    }

}