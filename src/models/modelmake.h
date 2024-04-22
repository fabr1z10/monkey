#pragma once

#include "../model.h"
#include "../shape.h"
#include "../symbols.h"
#include <pybind11/pybind11.h>
//#include "../batch/linebatch.h"

// convert shape to models
class ModelMaker {
public:
    static ModelMaker& instance() {
        static ModelMaker instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
	//static std::shared_ptr<Model> pippo(const pybind11::kwargs& args);
    static std::shared_ptr<Model> makeModel(const std::string& batchId, const std::shared_ptr<Shape>&, glm::vec4 color, FillType);
    std::shared_ptr<Model> make(const std::string& batchId, const std::shared_ptr<Shape>&, glm::vec4 color, FillType ft);
private:
    int m_pointsPerCirle;
    ModelMaker();
    std::shared_ptr<Model> makeConvexPoly(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
    std::shared_ptr<Model> makeCircle(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
    std::shared_ptr<Model> makeCompoundShape(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
    std::shared_ptr<Model> makeAABB(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
	std::shared_ptr<Model> makeAABB3D(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
	std::shared_ptr<Model> makePoly(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
	std::shared_ptr<Model> makePolyLine(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);

	std::shared_ptr<Model> makeAABBSolid(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);
	std::shared_ptr<Model> makePolySolid(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft);

	//std::shared_ptr<Model> makeAABB3D(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
    //std::shared_ptr<Model> makePrism(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
    typedef std::shared_ptr<Model> (ModelMaker::*funcPtrOne)(const std::shared_ptr<Shape>&, glm::vec4, FillType);
    std::unordered_map<std::type_index, funcPtrOne> _dss;
	std::unordered_map<std::type_index, funcPtrOne> _dssolid;
    std::string _batchId;
	//std::unordered_map<std::type_index, std::function<std::shared_ptr<Model>(const std::shared_ptr<IBatch>&, std::shared_ptr<Shape>, glm::vec4, FillType)>> m_builders;
};
