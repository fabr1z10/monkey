#include "lines.h"
#include "../pyhelper.h"
#include "../engine.h"

PolyChain::PolyChain(int batch, const std::vector<glm::vec2>& points, glm::vec4 color) : Model(), _lineCount(0) {
    //_batch = dynamic_cast<LineBatch*>(Engine::instance().getBatch(batch));
    glm::vec3 P0 (points[0], 0.f);
    for (size_t i = 1; i < points.size(); ++i) {
        glm::vec3 P1(points[i], 0.f);
        Segment s;
        s.P0 = P0;
        s.P1 = P1;
        s.color =color;
        _segments.push_back(s);
        P0=P1;
        _lineCount++;
    }
}

PolyChain::PolyChain(const pybind11::kwargs& args) : Model(), _batch(nullptr), _lineCount(0) {


    _batch = dynamic_cast<LineBatch*>(args["batch"].cast<std::shared_ptr<IBatch>>().get());
    auto z = py_get_dict<float>(args, "z", 0.f);
    auto py_pts = args["points"].cast<std::vector<float>>();
    auto color = py_get_dict<glm::vec4>(args, "color");
    glm::vec3 P0 (py_pts[0], py_pts[1], z);
    for (size_t i = 2; i < py_pts.size(); i+=2) {
        glm::vec3 P1 (py_pts[i], py_pts[i+1], z);
        Segment s;
        s.P0 = P0;
        s.P1 = P1;
        s.color = glm::vec4(1.f);
        _segments.push_back(s);
        P0 = P1;
        _lineCount++;
    }


}
std::shared_ptr<Renderer> PolyChain::getRenderer() const {
    return std::make_shared<PolyChainRenderer>(_batch);

}
PolyChainRenderer::PolyChainRenderer(LineBatch* batch) : Renderer(0, 0), _lineBatch(batch) {

}



void PolyChainRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs&) {
    _chain = std::dynamic_pointer_cast<PolyChain>(model);

    auto qc = _chain->getLineCount();
    for (int i = 0; i < qc; ++i) {
        _quadIds.push_back(_lineBatch->getPrimitiveId());
    }

}


std::type_index PolyChainRenderer::getType() {
    return std::type_index(typeid(Renderer));
}

void PolyChainRenderer::update(double dt) {

    // get world pos
    auto pos = m_node->getWorldPosition();


    for (size_t i = 0; i < _quadIds.size(); ++i) {
        const auto& s = _chain->getSegment(i);
        _lineBatch->setLine(_quadIds[i], pos + s.P0, pos + s.P1, s.color);
    }


}