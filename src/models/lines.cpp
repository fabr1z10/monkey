#include "lines.h"
#include "../pyhelper.h"
#include "../engine.h"

LineModel::LineModel(const std::string& batchId) : _lineCount(0), _batchId(batchId) {
    //_batch = static_cast<LineBatch*>(Engine::instance().getBatch(1, 0));
}

LineModel::LineModel(const std::string& batchId, const pybind11::kwargs & args) : LineModel(batchId) {
    auto color = args["color"].cast<glm::vec4>();
    auto points = args["points"].cast<std::vector<float>>();
    init(color, points);
}

void LineModel::setColor(glm::vec4 color) {
    for (auto& s : _segments) {
        s.color = color;
    }
}

void LineModel::init(const glm::vec4 &color, const std::vector<float> &data) {
    for (size_t i = 0; i < data.size(); i += 6) {
        size_t offset = i;
        Segment s;
        s.P0 = glm::vec3(data[offset], data[offset+1], data[offset+2]);
        s.P1 = glm::vec3(data[offset+3], data[offset+4], data[offset+5]);
        s.color = color;
        _segments.push_back(s);
        _lineCount++;
    }
}

std::shared_ptr<Renderer> LineModel::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<LinesRenderer>(_batchId);
}


PolyChain::PolyChain(const std::string& batchId, const pybind11::kwargs& args) : LineModel(batchId) {
    auto color = args["color"].cast<glm::vec4>();
    auto points = args["points"].cast<std::vector<float>>();
    auto closed = py_get_dict<bool>(args, "closed", true);
    initChain(color, points, closed);
}

void PolyChain::initChain(const glm::vec4 &color, const std::vector<float> &data, bool closed) {

    glm::vec3 P0(data[0], data[1], data[2]);
    glm::vec3 initialPoint = P0;
    for (size_t i = 3; i < data.size(); i += 3) {
        glm::vec3 P1(data[i], data[i+1], data[i+2]);
        _segments.push_back({P0, P1, color});
        P0 = P1;
        _lineCount++;
    }
    if (closed) {
        _segments.push_back({P0, initialPoint, color});
        _lineCount++;
    }
}

//PolyChain::PolyChain(const pybind11::kwargs& args) : Model(), _batch(nullptr), _lineCount(0) {
//
//
//    _batch = dynamic_cast<LineBatch*>(args["batch"].cast<std::shared_ptr<IBatch>>().get());
//    auto z = py_get_dict<float>(args, "z", 0.f);
//    auto py_pts = args["points"].cast<std::vector<float>>();
//    auto color = py_get_dict<glm::vec4>(args, "color");
//    glm::vec3 P0 (py_pts[0], py_pts[1], z);
//    for (size_t i = 2; i < py_pts.size(); i+=2) {
//        glm::vec3 P1 (py_pts[i], py_pts[i+1], z);
//        Segment s;
//        s.P0 = P0;
//        s.P1 = P1;
//        s.color = glm::vec4(1.f);
//        _segments.push_back(s);
//        P0 = P1;
//        _lineCount++;
//    }
//
//
//}

LinesRenderer::LinesRenderer(const std::string& batchId) : BatchRenderer<LineBatch>(batchId, pybind11::kwargs()) {
    //assert(_lineBatch);

}



void LinesRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs&) {
    _lineModel = std::dynamic_pointer_cast<LineModel>(model);
    auto qc = _lineModel->getLineCount();
    for (int i = 0; i < qc; ++i) {
        _primitiveIds.push_back(_batch->getPrimitiveId());
    }

}

std::type_index LinesRenderer::getType() {
    return std::type_index(typeid(Renderer));
}

void LinesRenderer::update(double dt) {

    // get world pos
    auto pos = m_node->getWorldMatrix();


    for (size_t i = 0; i < _primitiveIds.size(); ++i) {
        const auto& s = _lineModel->getSegment(i);
        auto a = glm::vec3(pos * glm::vec4(s.P0, 1.f));
		auto b = glm::vec3(pos * glm::vec4(s.P1, 1.f));
        _batch->setLine(_primitiveIds[i], a, b, s.color);
    }


}

