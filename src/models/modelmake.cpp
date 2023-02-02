#include "modelmake.h"
#include <iostream>
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"

#include "../shapes/compound.h"
#include "../shapes/aabb.h"
#include "../pyhelper.h"
#include "../models/multi.h"


#include <cmath>

ModelMaker::ModelMaker() : m_pointsPerCirle(20) {
    m_builders[std::type_index(typeid(Rect))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeConvexPoly(s, color, ft); };
    m_builders[std::type_index(typeid(Segment))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeConvexPoly(s, color, ft); };
    m_builders[std::type_index(typeid(Circle))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeCircle(s, color, ft); };
    m_builders[std::type_index(typeid(ConvexPoly))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeConvexPoly(s, color, ft); };
    m_builders[std::type_index(typeid(CompoundShape))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeCompoundShape(s, color, ft); };
    m_builders[std::type_index(typeid(AABB))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeAABB(s, color, ft); };
    //m_builders[std::type_index(typeid(AABB3D))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makeAABB3D(s, args); };
    //m_builders[std::type_index(typeid(Prism))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makePrism(s, args); };

}

std::shared_ptr<Model> ModelMaker::makeCompoundShape(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) {
    auto* cs = static_cast<CompoundShape*>(s.get());
    auto model = std::make_shared<MultiModel>();
    for (const auto& shape : cs->getShapes()) {
        model->addModel(this->make(shape, color, ft));
    }
    return model;
}

std::shared_ptr<Model> ModelMaker::makeAABB(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) {
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    auto b = s->getBounds();
    vertices.insert(vertices.end(), {b.min.x, b.min.y, 0.f, color.r, color.g, color.b, color.a});
    vertices.insert(vertices.end(), {b.max.x, b.min.y, 0.f, color.r, color.g, color.b, color.a});
    vertices.insert(vertices.end(), {b.max.x, b.max.y, 0.f, color.r, color.g, color.b, color.a});
    vertices.insert(vertices.end(), {b.min.x, b.max.y, 0.f, color.r, color.g, color.b, color.a});

    GLuint prim {GL_LINE_LOOP};
    if (ft == FillType::OUTLINE) {
        elements.insert(elements.end(), {0, 1, 2, 3});
    } else {
        elements.insert(elements.end(), {0, 1, 2, 2, 3, 0});
        prim = GL_TRIANGLES;
    }

    auto model = std::make_shared<Model>(ShaderType::SHADER_COLOR, prim);
    model->generateBuffers(vertices, elements);
    return model;
}

std::shared_ptr<Model> ModelMaker::makeConvexPoly(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) {
    auto* cp = static_cast<ConvexPoly*>(s.get());
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    const auto& points = cp->getPoints();
    for (const auto& p : points) {
        vertices.insert(vertices.end(), {p.x, p.y, 0.f, color.r, color.g, color.b, color.a});
        //elements.push_back(u++);
    }

    GLuint prim {GL_LINE_LOOP};
    if (ft == FillType::OUTLINE) {
        for (int i = 0; i < points.size(); ++i) {
            elements.push_back(i);
        }
        elements.push_back(0);
    } else {
        prim = GL_TRIANGLES;
        for (int i = 0; i < points.size() - 2; ++i) {
            elements.push_back(0);
            elements.push_back(i + 1);
            elements.push_back(i + 2);
        }
    }

    auto model = std::make_shared<Model>(ShaderType::SHADER_COLOR, prim);
    model->generateBuffers(vertices, elements);
    return model;

}

std::shared_ptr<Model> ModelMaker::makeCircle(std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) {
    auto* c = static_cast<Circle*>(s.get());
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    auto center = c->getOffset();
    auto radius = c->getRadius();
    float delta = 2.0f * M_PI / m_pointsPerCirle;
    float angle = 0.0f;
    if (ft == FillType::SOLID) {
        vertices.insert(vertices.end(), {center.x, center.y, 0.f, color.r, color.g, color.b, color.a});
    }
    for (int i = 0; i < m_pointsPerCirle; ++i) {
        vertices.insert(vertices.end(), {center.x + radius * cos(angle), center.y + radius * sin(angle), 0.0f, color.r, color.g, color.b, color.a});
        angle += delta;
        //elements.push_back(u++);
    }

    GLuint prim {GL_LINE_LOOP};
    if (ft == FillType::OUTLINE) {
        for (int i = 0; i < vertices.size(); ++i) {
            elements.push_back(i);
        }
        elements.push_back(0);
    } else {
        prim = GL_TRIANGLES;
        for (int i = 0; i < vertices.size() - 2; ++i) {
            elements.push_back(0);
            elements.push_back(i + 1);
            elements.push_back(i + 2);
        }
        elements.back() = 1;
    }


    auto model = std::make_shared<Model>(ShaderType::SHADER_COLOR, prim);
    model->generateBuffers(vertices, elements);
    return model;

}


std::shared_ptr<Model> ModelMaker::make(std::shared_ptr<Shape> shape, glm::vec4 color, FillType ft) {

    auto it = m_builders.find(shape->get_type_index());
    if (it != m_builders.end()) {
        return it->second(shape, color, ft);
    }
    std::cout << " --- don't know how to build model for shape: " << shape->get_type_index().name() << "\n";
    return nullptr;

}