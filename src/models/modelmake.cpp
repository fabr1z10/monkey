#include "modelmake.h"
#include <iostream>
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"

#include "../shapes/compound.h"
#include "../shapes/aabb.h"
#include "../shapes/aabb3d.h"
#include "../shapes/polygon.h"
#include "../shapes/polyline.h"
#include "../pyhelper.h"
//#include "../models/multi.h"
#include "../models/lines.h"

#include "lines.h"
#include "triangles.h"


#include <cmath>

ModelMaker::ModelMaker() : m_pointsPerCirle(20) {
    _dss[std::type_index(typeid(Rect))] = &ModelMaker::makeConvexPoly;
    _dss[std::type_index(typeid(Segment))] = &ModelMaker::makeConvexPoly;
    _dss[std::type_index(typeid(Circle))] = &ModelMaker::makeCircle;
    _dss[std::type_index(typeid(ConvexPoly))] = &ModelMaker::makeConvexPoly;
    _dss[std::type_index(typeid(CompoundShape))] = &ModelMaker::makeCompoundShape;
    _dss[std::type_index(typeid(AABB))] = &ModelMaker::makeAABB;
	_dss[std::type_index(typeid(AABB3D))] = &ModelMaker::makeAABB3D;
	_dss[std::type_index(typeid(GenericPolygon))] = &ModelMaker::makePoly;
	_dss[std::type_index(typeid(PolyLine))] = &ModelMaker::makePolyLine;

	_dssolid[std::type_index(typeid(GenericPolygon))] = &ModelMaker::makePolySolid;
	_dssolid[std::type_index(typeid(AABB))] = &ModelMaker::makeAABBSolid;

	//
//    m_builders[std::type_index(typeid(Rect))] = &ModelMaker::makeConvexPoly; // [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeConvexPoly(s, color, ft); };
//    m_builders[std::type_index(typeid(Segment))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeConvexPoly(s, color, ft); };
//    m_builders[std::type_index(typeid(Circle))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeCircle(s, color, ft); };
//    m_builders[std::type_index(typeid(ConvexPoly))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeConvexPoly(s, color, ft); };
//    m_builders[std::type_index(typeid(CompoundShape))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeCompoundShape(s, color, ft); };
//    m_builders[std::type_index(typeid(AABB))] = [&] (std::shared_ptr<Shape> s, glm::vec4 color, FillType ft) { return makeAABB(s, color, ft); };
//    //m_builders[std::type_index(typeid(AABB3D))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makeAABB3D(s, args); };
//    //m_builders[std::type_index(typeid(Prism))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makePrism(s, args); };

}

std::shared_ptr<Model> ModelMaker::makeModel(const std::string& batchId, const std::shared_ptr<Shape> & shape, glm::vec4 color, FillType fillType) {
    return ModelMaker::instance().make(batchId, shape, color, fillType);
}

std::shared_ptr<Model> ModelMaker::make(const std::string& batchId, const std::shared_ptr<Shape>& shape, glm::vec4 color, FillType ft) {
    _batchId = batchId;
	auto& op = ft == FillType::OUTLINE ? _dss : _dssolid;
	auto it = op.find(shape->get_type_index());
	if (it != op.end()) {
		return ((*this).*(it->second))(shape, color, ft);
	}
	std::cout << " --- don't know how to build model for shape: " << shape->get_type_index().name() << "\n";
    return nullptr;

}
std::shared_ptr<Model> ModelMaker::makeCompoundShape(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft) {
//    auto* cs = static_cast<CompoundShape*>(s.get());
//    auto model = std::make_shared<MultiModel>();
//    for (const auto& shape : cs->getShapes()) {
//        model->addModel(this->make(shape, color, ft));
//    }
//    return model;
}


std::shared_ptr<Model> ModelMaker::makePolyLine(const std::shared_ptr<Shape> &s, glm::vec4 color, FillType ft) {
	auto* p = static_cast<PolyLine*>(s.get());
	const auto& outline = p->getSegments();
	std::vector<float> data;
	for (const auto seg : *outline) {
		data.push_back(seg.P0.x);
		data.push_back(seg.P0.y);
		data.push_back(0.f);
	}
	data.push_back(outline->back().P1.x);
	data.push_back(outline->back().P1.y);
	data.push_back(0.f);
	auto lines = std::make_shared<PolyChain>(_batchId);
	lines->initChain(color, data, false);//1, pts, glm::vec4(1.f));
	return lines;
}


std::shared_ptr<Model> ModelMaker::makeAABBSolid(const std::shared_ptr<Shape> &s, glm::vec4 color, FillType ft) {

	auto bounds = s->getBounds();
	std::vector<float> data {bounds.min.x, bounds.min.y, bounds.max.x, bounds.min.y,
						  bounds.max.x, bounds.max.y, bounds.min.x, bounds.max.y};
	auto triangles = std::make_shared<TrianglesModel>(_batchId);
	triangles->init(color, data);//1, pts, glm::vec4(1.f));
	return triangles;

}

std::shared_ptr<Model> ModelMaker::makePolySolid(const std::shared_ptr<Shape> &s, glm::vec4 color, FillType ft) {

	std::vector<float> data;
	auto* p = static_cast<GenericPolygon*>(s.get());
	const auto& outline = p->getOutline();
	for (const auto& point : outline) {
		data.push_back(point.x);
		data.push_back(point.y);
	}

	auto triangles = std::make_shared<TrianglesModel>(_batchId);
	triangles->init(color, data);//1, pts, glm::vec4(1.f));
	return triangles;
}

std::shared_ptr<Model> ModelMaker::makePoly(const std::shared_ptr<Shape> &s, glm::vec4 color, FillType ft) {
	auto* p = static_cast<GenericPolygon*>(s.get());
	const auto& outline = p->getOutline();
	std::vector<float> data;
	for (const auto& point : outline) {
		data.push_back(point.x);
		data.push_back(point.y);
		data.push_back(0.f);
	}
	auto lines = std::make_shared<PolyChain>(_batchId);
	lines->initChain(color, data, true);//1, pts, glm::vec4(1.f));

	for (size_t i = 0; i < p->getHoleCount(); ++i) {
		auto& c = p->getHoleOutline(i);
		std::vector<float> d;
		for (const auto& point : c) {
			d.push_back(point.x);
			d.push_back(point.y);
			d.push_back(0.f);
		}
		lines->initChain(color, d, true);//1, pts, glm::vec4(1.f));

	}
	return lines;
}

std::shared_ptr<Model> ModelMaker::makeAABB(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft) {

    auto b = s->getBounds();
    std::vector<float> data = {
        b.min.x, b.min.y, 0.f,
        b.max.x, b.min.y, 0.f,
        b.max.x, b.max.y, 0.f,
        b.min.x, b.max.y, 0.f
    };
    auto lines = std::make_shared<PolyChain>(_batchId);
    lines->initChain(color, data, true);//1, pts, glm::vec4(1.f));

    return lines;
}

std::shared_ptr<Model> ModelMaker::makeAABB3D(const std::shared_ptr<Shape> &s, glm::vec4 color, FillType ft) {
	auto b = s->getBounds();
	std::vector<float> data = {
		b.min.x, b.min.y, b.min.z,
		b.max.x, b.min.y, b.min.z,
		b.max.x, b.min.y, b.min.z,
		b.max.x, b.min.y, b.max.z,
		b.max.x, b.min.y, b.max.z,
		b.min.x, b.min.y, b.max.z,
		b.min.x, b.min.y, b.max.z,
		b.min.x, b.min.y, b.min.z,
		b.min.x, b.max.y, b.min.z,
		b.max.x, b.max.y, b.min.z,
		b.max.x, b.max.y, b.min.z,
		b.max.x, b.max.y, b.max.z,
		b.max.x, b.max.y, b.max.z,
		b.min.x, b.max.y, b.max.z,
		b.min.x, b.max.y, b.max.z,
		b.min.x, b.max.y, b.min.z,
		b.min.x, b.min.y, b.min.z,
		b.min.x, b.max.y, b.min.z,
		b.max.x, b.min.y, b.min.z,
		b.max.x, b.max.y, b.min.z,
		b.min.x, b.min.y, b.max.z,
		b.min.x, b.max.y, b.max.z,
		b.max.x, b.min.y, b.max.z,
		b.max.x, b.max.y, b.max.z,

	};
	auto lines = std::make_shared<LineModel>(_batchId);
	lines->init(color, data);
	//lines->initChain(color, data, true);//1, pts, glm::vec4(1.f));

	return lines;
}

std::shared_ptr<Model> ModelMaker::pippo(const pybind11::kwargs& args) {
//	auto size = py_get_dict<glm::vec2>(args, "size");
//	auto tex_repeat = py_get_dict<glm::vec2>(args, "tex_repeat", glm::vec2(0.f));
//
//	auto sheetFile = args["tex"].cast<std::string>();
//
//	if (tex_repeat == glm::vec2(0.f)) {
//		auto tex_repeat_every = py_get_dict<glm::vec2>(args, "tex_period", glm::vec2(0.f));
//		tex_repeat.x = size.x / tex_repeat_every.x;
//		tex_repeat.y = size.y / tex_repeat_every.y;
//	}
//	float width = size.x;
//	float height = size.y;
//	float tw = tex_repeat.x;
//	float th = tex_repeat.y;
//	auto normal = py_get_dict<char>(args, "normal");
//	std::vector<float> vertices;
//
//	if (normal == 'x') {
//		vertices = std::vector<float>({
//			0.f, 0.f, width, 0.f, 0.f, 1.f, 0.f, 0.f,
//			0.f, 0.f, 0.f, tw, 0.f, 1.f, 0.f, 0.f,
//			0.f, height, 0.f, tw, th, 1.f, 0.f, 0.f,
//			0.f, height, width, 0.f, th, 1.f, 0.f, 0.f});
//	} else if (normal == 'y') {
//		vertices = std::vector<float>({
//		    0.f, 0.f, height, 0.f, 0.f, 0.f, 1.f, 0.f,
//			width, 0.f, height, tw, 0.f, 0.f, 1.f, 0.f,
//			width, 0.f, 0.f, tw, th, 0.f, 1.f, 0.f,
//			0.f, 0.f, 0.f, 0.f, th, 0.f, 1.f, 0.f});
//	} else if (normal == 'z') {
//		vertices = std::vector<float>({
//			0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
//			width, 0.f, 0.f, tw, 0.f, 0.f, 0.f, 1.f,
//			width, height, 0.f, tw, th, 0.f, 0.f, 1.f,
//			0.f, height, 0.f, 0.f, th, 0.f, 0.f, 1.f});
//	} else {
//		std::cerr << "unknown normal: " << normal << ", must be x, y or z.";
//		exit(1);
//	}
//	std::vector<unsigned> elements{0, 1, 2, 3, 0, 2};
//	auto model = std::make_shared<Model>();
//	//model->generateBuffers(vertices, elements);
//	//model->setTexture(sheetFile);
//	return model;

}

std::shared_ptr<Model> ModelMaker::makeConvexPoly(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft) {
    auto* cp = static_cast<ConvexPoly*>(s.get());
    std::vector<float> data;
//    std::vector<unsigned> elements;
//    unsigned u{0};
    const auto& points = cp->getPoints();
    bool closed = points.size() > 2;
    for (const auto& p : points) {
        data.insert(data.end(), {p.x, p.y, 0.f});
    }
//
//    GLuint prim {GL_LINE_LOOP};
//    if (ft == FillType::OUTLINE) {
//        for (int i = 0; i < points.size(); ++i) {
//            elements.push_back(i);
//        }
//        elements.push_back(0);
//    } else {
//        prim = GL_TRIANGLES;
//        for (int i = 0; i < points.size() - 2; ++i) {
//            elements.push_back(0);
//            elements.push_back(i + 1);
//            elements.push_back(i + 2);
//        }
//    }
//
//    auto model = std::make_shared<Model>();
//    //model->generateBuffers(vertices, elements);
//    return model;

    auto lmodel = std::make_shared<PolyChain>(_batchId);
    lmodel->initChain(color, data, closed);

    return lmodel;
}

std::shared_ptr<Model> ModelMaker::makeCircle(const std::shared_ptr<Shape>& s, glm::vec4 color, FillType ft) {
    auto* c = static_cast<Circle*>(s.get());
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    auto center = c->getOffset();
    auto radius = c->getRadius();
    float delta = 2.0f * M_PI / m_pointsPerCirle;
    float angle = 0.0f;


    auto lmodel = std::make_shared<PolyChain>(_batchId);

    std::vector<float> data;

    for (int i = 0; i < m_pointsPerCirle; ++i) {
        data.insert(data.end(), {center.x + radius * cos(angle), center.y + radius * sin(angle), 0.0f});
        angle += delta;
    }
    lmodel->initChain(color, data, true);
//
//
//    GLuint prim {GL_LINE_LOOP};
//    if (ft == FillType::OUTLINE) {
//        for (int i = 0; i < vertices.size(); ++i) {
//            elements.push_back(i);
//        }
//        elements.push_back(0);
//    } else {
//        prim = GL_TRIANGLES;
//        for (int i = 0; i < vertices.size() - 2; ++i) {
//            elements.push_back(0);
//            elements.push_back(i + 1);
//            elements.push_back(i + 2);
//        }
//        elements.back() = 1;
//    }
//
//
//    auto model = std::make_shared<Model>();
//    //model->generateBuffers(vertices, elements);
    return lmodel;

}


