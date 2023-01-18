#pragma once

#include "../model.h"
#include "../components/renderer.h"

struct TiledFrameInfo {
    TiledFrameInfo(int a, int b, int c) : offset(a), count(b), ticks(c) {}
    unsigned offset;
    unsigned count;
    int ticks;
};

class TiledModel : public Model {
public:
    TiledModel();
    TiledModel(const pybind11::kwargs&);
protected:
    void readSheet(const pybind11::kwargs&);
    void readTiles(const std::string& inputString, std::vector<GLfloat>& vertices, std::vector<unsigned>& indices);
    glm::vec2 m_tileSize;
    float m_t1, m_t2;
    unsigned m_i0;
};
