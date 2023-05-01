#pragma once

#include "../model.h"
#include "../components/renderer.h"
#include "../linebatch.h"


class PolyChain : public Model {
private:
    struct Segment {
        glm::vec3 P0;
        glm::vec3 P1;
        glm::vec4 color;
    };
public:
    PolyChain(int batch, const std::vector<glm::vec2>& points, glm::vec4 color);
    explicit PolyChain(const pybind11::kwargs&);
    std::shared_ptr<Renderer> getRenderer() const override;
    int getLineCount() const;
    const Segment& getSegment(int index) const;
private:


    LineBatch* _batch;
    std::vector<Segment> _segments;
    int _lineCount;
};

inline int PolyChain::getLineCount() const {
    return _lineCount;
}

inline const PolyChain::Segment& PolyChain::getSegment(int index) const {
    return _segments[index];
}

class PolyChainRenderer : public Renderer {
public:
    explicit PolyChainRenderer(LineBatch*);
    void setModel(std::shared_ptr<Model>) override;
    std::type_index getType() override;
    //void start() override;
    void update(double) override;
private:
    LineBatch* _lineBatch;
    std::shared_ptr<PolyChain> _chain;
    std::vector<int> _quadIds;
};


