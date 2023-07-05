#pragma once

#include "../model.h"
#include "../components/renderer.h"
#include "../batch/quadbatch.h"


class QuadBase : public Model {
public:


};


class Quad : public Model {
public:
    struct Desc {
        glm::vec3 pos;
        glm::vec2 size;
        glm::vec2 repeat;
        glm::vec4 textureCoordinates;
        bool flipv;
        bool fliph;
        unsigned paletteIndex;
    };

    struct Frame {
        std::vector<Desc> quadDesc;
        int ticks;
    };
    explicit Quad(const pybind11::kwargs&);
    const Frame& getFrame(int) const;
    int getQuadCount() const;
    std::shared_ptr<Renderer> getRenderer(IBatch*) override;
    int getFrameCount() const;
private:
    //QuadBatch* _batch;
    std::vector<Frame> _frames;
    int _quadCount;
};

inline int Quad::getFrameCount() const {
    return _frames.size();
}

inline const Quad::Frame& Quad::getFrame(int id) const {
    return _frames[id];
};

inline int Quad::getQuadCount() const {
    return _quadCount;
}

class QuadRenderer : public Renderer {
public:
    explicit QuadRenderer(IBatch*);
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs&) override;
    std::type_index getType() override;
    void start() override;
    void update(double) override;
private:
	QuadBatch* _spriteBatch;
    int _frame;
    int _ticks;
    std::shared_ptr<Quad> _quad;
    std::vector<int> _quadIds;

};


