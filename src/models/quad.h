#pragma once

#include "../model.h"
#include "../components/renderer.h"
#include "../spritebatch.h"


class Quad : public Model {
public:
    struct Desc {
        glm::vec3 pos;
        glm::vec2 size;
        glm::vec2 repeat;
        glm::vec4 textureCoordinates;
        unsigned paletteIndex;
    };

    struct Frame {
        std::vector<Desc> quadDesc;
        int ticks;
    };
    explicit Quad(const pybind11::kwargs&);
    const Frame& getFrame(int) const;
    int getQuadCount() const;
    std::shared_ptr<Renderer> getRenderer() const override;

private:
    SpriteBatch* _batch;
    std::vector<Frame> _frames;
    int _quadCount;
};

inline const Quad::Frame& Quad::getFrame(int id) const {
    return _frames[id];
};

inline int Quad::getQuadCount() const {
    return _quadCount;
}

class QuadRenderer : public Renderer {
public:
    explicit QuadRenderer(SpriteBatch*);
    void setModel(std::shared_ptr<Model>) override;
    std::type_index getType() override;
    void start() override;
    void update(double) override;
private:
    SpriteBatch* _spriteBatch;
    int _frame;
    int _ticks;
    std::shared_ptr<Quad> _quad;
    std::vector<int> _quadIds;

};


