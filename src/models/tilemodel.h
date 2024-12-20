#pragma once

#include "../model.h"
#include <pybind11/pybind11.h>
#include <iostream>
#include "../batch/quadbatch.h"
#include "../components/batchrenderer.h"


struct TileInfo {
    TileInfo(bool visible) : visible(visible) {}

    TileInfo(glm::vec4 tc, int pal, bool flipv, bool fliph) : visible(true),
        textureCoordinates(tc), palette(pal), flipv(flipv), fliph(fliph) {}

    bool visible;

    glm::vec4 textureCoordinates;

    int palette;

    bool flipv;

    bool fliph;


};

class TileModel : public Model {
public:
    TileModel(const std::string& batchId, int width, int height, int frames, int ticksPerFrame, int scale);

    int addTile(int tileNumber, int pal, bool fliph, bool flipv);

    void setTile(int pos, int tick, int tileId);

    const TileInfo* getTile (int pos, int tick);

    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;

    int getQuadCount() const;

    int getFrames() const;

    int getTicksPerFrame() const;

    int getWidth() const;

private:
    std::string _batchId;
    std::vector<TileInfo> _tiles;
    int _tilesInWidth;
    int _tilesInHeight;
    glm::ivec2 _tileSize;
    std::vector<int> _frameInfo;
    int _width;
    int _height;
    int _quadCount;
    int _frames;
    int _tickPerFrames;
    int _scale;
};

inline int TileModel::getQuadCount() const {
    return _quadCount;
}
inline int TileModel::getFrames() const {
    return _frames;
}
inline int TileModel::getTicksPerFrame() const {
    return _tickPerFrames;
}

inline int TileModel::getWidth() const {
    return _width;
}


class TileModelRenderer : public BatchRenderer<QuadBatch> {
public:
    explicit TileModelRenderer(const std::string& batchId, float scale);
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
    std::type_index getType() override;
    //void start() override;

    // nothing to do in update! (no animation)
    void update(double) override ;
private:
    glm::vec2 _tileSize;
    unsigned _camId;
    TileModel* _model;
    float _fade;
    int _tick;
    int _ticksPerFrame;
    int _frames;
    int _frame;
    int _quadCount;
    int _width;
};