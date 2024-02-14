#pragma once

#include "../model.h"
#include "../components/batchrenderer.h"
#include "../batch/linebatch.h"


class LineModel : public Model {
public:
    struct Segment {
        glm::vec3 P0;
        glm::vec3 P1;
        glm::vec4 color;
    };
    explicit LineModel(const std::string& batchId);
    explicit LineModel(const std::string& batchId, const pybind11::kwargs&);
    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
    int getLineCount() const;
    const Segment& getSegment(int index) const;
    void init(const glm::vec4& color, const std::vector<float>& data);
    void setColor(glm::vec4 color) override;
protected:
    std::string _batchId;
    //LineBatch* _batch;
    std::vector<Segment> _segments;
    int _lineCount;
};

inline const LineModel::Segment& LineModel::getSegment(int index) const {
    return _segments[index];
}

inline int LineModel::getLineCount() const {
    return _lineCount;
}


class PolyChain : public LineModel {
public:
    explicit PolyChain(const std::string& batchId) : LineModel(batchId) {}
    explicit PolyChain(const std::string& batchId, const pybind11::kwargs&);
    void initChain(const glm::vec4& color, const std::vector<float>& data, bool closed);
};

class LinesRenderer : public BatchRenderer<LineBatch> {
public:
    explicit LinesRenderer(const std::string&);
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs&) override;
    std::type_index getType() override;
    //void start() override;
    void update(double) override;
private:
    std::shared_ptr<LineModel> _lineModel;
};

