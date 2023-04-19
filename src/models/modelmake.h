#pragma once

#include "../model.h"
#include "../shape.h"
#include "../symbols.h"

// convert shape to models
class ModelMaker {
public:
    static ModelMaker& instance() {
        static ModelMaker instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
	static std::shared_ptr<Model> pippo(const pybind11::kwargs& args);
    std::shared_ptr<Model> make(std::shared_ptr<Shape>, glm::vec4 color, FillType ft);
private:
    int m_pointsPerCirle;
    ModelMaker();
    std::shared_ptr<Model> makeConvexPoly(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft);
    std::shared_ptr<Model> makeCircle(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft);
    std::shared_ptr<Model> makeCompoundShape(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft);
    std::shared_ptr<Model> makeAABB(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft);
    //std::shared_ptr<Model> makeAABB3D(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
    //std::shared_ptr<Model> makePrism(std::shared_ptr<Shape> s, const pybind11::kwargs& args);

    std::unordered_map<std::type_index, std::function<std::shared_ptr<Model>(std::shared_ptr<Shape>, glm::vec4, FillType)>> m_builders;
};
