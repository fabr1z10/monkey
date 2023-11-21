#pragma once

#include "../model.h"
#include "../components/batchrenderer.h"
#include "../batch/quadbatch.h"

struct Tile {
    int x, y;
    glm::vec2 position;
    glm::ivec4 grid;

};

class TileMap : public Model {
public:

    explicit TileMap(const pybind11::kwargs& args);
    glm::ivec2 getDeviceSize() const;
    int getTileSize() const;
    int getLayerCount() const;
    const std::unordered_set<int>* getTiles(int x, int y);
    void move(int, float x, float y);
    void placeOnGrid(int, float x, float y);
    Tile& getTile(int id ) {return _tiles[id];}
    void setTile(int x, int y, int i, int j, glm::vec2 offset);
    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;

private:

    std::unordered_map<glm::ivec2, std::unordered_set<int>> _grid;
    std::vector<Tile> _tiles;
    glm::ivec2 _deviceSize;
    int _tileSize;
    int _layers;

};

inline glm::ivec2 TileMap::getDeviceSize() const { return _deviceSize; }

inline int TileMap::getTileSize() const {
    return _tileSize;
}

inline int TileMap::getLayerCount() const {
    return _layers;
}


class TileMapRenderer : public BatchRenderer<QuadBatch> {
public:
    explicit TileMapRenderer(const pybind11::kwargs& args);

    void setModel(std::shared_ptr<Model>, const pybind11::kwargs&) override;
    void update(double) override;


private:
    TileMap* _tileMap;
    //Node* _player;
    // bottom left corner of viewing area
    glm::ivec2 _viewArea;
    glm::ivec2 _deviceSize;
    glm::ivec2 _worldSize;
    glm::ivec2 _sheetSize;
    glm::vec2 _invSheetSize;
    int _tileSize;
    int _columns;
    int _rows;
    double _timer=0.;

};

