#include "walkarea.h"
#include "../pyhelper.h"
#include "../models/modelmake.h"
#include "../components/hotspot.h"
#include "../shapes/aabb.h"
#include <iostream>
#include "../util.h"
#include "../engine.h"
#include "../error.h"
#include "../runners/scheduler.h"
#include "../actions/walk.h"

using namespace pybind11::literals; // to bring in the `_a` literal

WalkArea::WalkArea(const pybind11::kwargs &args) : Node(), _zFunc(nullptr), _scaleFunc(nullptr) {
	_margin = py_get_dict<float>(args, "margin", 0.1f);



	// REQUIREMENT:
	// polygon outline must be provided ccw
	// holes cw order
	auto polygon = py_get_dict<std::vector<float>>(args, "poly");
	assert(polygon.size() % 2 == 0);

	_poly = std::make_shared<Polygon>(polygon);

	auto batchId = py_get_dict<std::string>(args, "batch", "");
	if (!batchId.empty()) {
		auto model = ModelMaker::makeModel(_poly, glm::vec4(1.0f, 1.f, 1.f, 1.f));
		Node::setModel(model, pybind11::dict("batch"_a = batchId));
	}

	// walkares don't need to be hotspot
	// you just need to have a hotspot as large as the room
	// when you click on that you create a WALK action that gets the walk area associated to the actor

//	auto hotspot = std::make_shared<HotSpot>(_poly, pybind11::dict("batch"_a = batchId));
//	hotspot->setOnClick([&] (glm::vec2 P, int btn, int action) {
//		onClick(P, btn, action);
//	});
//	addComponent(hotspot);


	// create graph
	_graph = std::make_shared<Graph<int, glm::vec2>>();
	int offset = 10;
	const auto& outline = _poly->getOutline();
	addPath(outline, false, offset);
	offset += outline.size();
	int holeCount = _poly->getHoleCount();
	for (size_t i =0 ; i< holeCount; ++i) {
		const auto& outline = _poly->getHoleOutline(i);
		addPath(outline, true, offset);
		offset += outline.size();
	}

	// prepare edge data
	glm::vec2 uvec = glm::normalize(outline.front() - outline.back());
	for (size_t i = 0; i < outline.size(); ++i) {
		EdgeData edge;
		edge.start = outline[i];
		edge.end = outline[i+1];
		edge.unitVec = glm::normalize(outline[i+1] - outline[i]);
		edge.normal = rot90(edge.unitVec, false);
		edge.length = glm::length(outline[i+1] - outline[i]);
		edge.normalInAtStart = glm::normalize(edge.unitVec - uvec);
		if (i > 0)
			_edgeData.back().normalInAtEnd = edge.normalInAtStart;
		uvec = edge.unitVec;
		_edgeData.push_back(edge);
	}

}

void WalkArea::onClick(glm::vec2 P, int button, int action) {
	std::cout << " clicked walkarea at " << P.x << ", " << P.y << ", " << button << ", " << action << "\n";
//
//	// check if player is on this walkarea
//	const auto& player = Engine::instance().getNodes("player");
//	if (player.empty() || player.size() > 2) {
//		GLIB_FAIL("There must be one and only one item tagged <player>!");
//	}
//	auto node = (*player.begin());
//	if (node->getParent() == this) {
//		std::cout << "ok\n";
		// crea lo script
		//
	auto script = std::make_shared<Script>( pybind11::dict("id"_a = "_player"));

	script->add(
			std::make_shared<Walk>(pybind11::dict("tag"_a = "player", "target"_a = P)),
			pybind11::dict());
	auto scheduler = Engine::instance().getRoom()->getRunner<Scheduler>();
	scheduler->add(script);

//
//		//node->setPosition(P.x, P.y, 0.f);
//	} else {
//		std::cout << "ko\n";
//	}

}

void WalkArea::addPath(const std::vector<glm::vec2>& path, bool isHole, int offset) {
	// consider special case of path made of only 2 points (segment)

	size_t ip = path.size() - 1;
	size_t ic = 0;			// index to current node
	size_t in = 1;			// index to next node
	do {
		glm::vec2 edgeNext = path[in] - path[ic];
		glm::vec2 edgePrev = path[ic] - path[ip];
		float flip = isHole ? -1.0f : 1.0f;
		auto f = cross2D(edgePrev, edgeNext) * flip;
		if (f < 0) {
			glm::vec2 u = glm::normalize(edgeNext);
			glm::vec2 v = glm::normalize(-edgePrev);
			glm::vec2 w = glm::normalize(u+v);
			addNode(offset, path[ic] - w * _margin);
			offset++;
		}
		ip = ic;
		ic = in;
		in = (in + 1) % path.size();

	} while (ic != 0);
}


int WalkArea::addNode(int id, glm::vec2 point) {
	// adds a node to the graph
	// and an edge from the new node to every other node that is in line of sight with it
	//int id = m_graph->getVertexCount();

	_graph->addNode(id, point);
	std::cout << "Added node " << id << " at " << point.x << ", " << point.y << "\n";
	float t{0.f};
	for (const auto& node : _graph->getNodes()) {
		if (node.first != id) {
			glm::vec2 P = node.second;
			if (!_poly->intersectSegment(point, P, t)) {
				_graph->addEdge(id, node.first, glm::length(point-P));
				std::cout << "Added edge from node " << id << " to node " << node.first << "\n";
			}
		}
	}
	return id;
}

//
//WalkAreaForwarder::WalkAreaForwarder(const pybind11::kwargs &args) {
//	auto size = py_get_dict<glm::vec2>(args, "size");
//	auto batchId = py_get_dict<std::string>(args, "batch", "");
//	auto shape = std::make_shared<AABB>(0, size.x, 0, size.y);
//	auto hotspot = std::make_shared<HotSpot>(shape, pybind11::dict("batch"_a = batchId));
//	hotspot->setOnClick([&] (glm::vec2 P, int btn, int action) {
//		onClick(P, btn, action);
//	});
//	addComponent(hotspot);
//}
//
//void WalkAreaForwarder::onClick(glm::vec2 P, int button, int action) {
//
//	// check if player is on this walkarea
//	const auto& player = Engine::instance().getNodes("player");
//	if (player.empty() || player.size() > 2) {
//		GLIB_FAIL("There must be one and only one item tagged <player>!");
//	}
//	auto node = (*player.begin());
//	auto walkArea = dynamic_cast<WalkArea*>(node->getParent());
//	if (walkArea != nullptr) {
//		walkArea->onClick(P, button, action);
//	}
//
//
//
//}

glm::vec2 WalkArea::closestPointIn(glm::vec2 P) {
	glm::vec3 p(P.x, P.y, 0.f);
	if (_poly->isInside(p)) {
		return P;
	}
	// if we get here P is not inside the poly
	glm::vec2 closestPointSoFar;
	float distanceSoFar = std::numeric_limits<float>::infinity();
	for (const auto& edge : _edgeData) {
		auto dp = glm::dot(P - edge.start, edge.unitVec);
		glm::vec2 currentPoint;
		float currentLength;
		glm::vec2 currentNormal;
		if (dp <= 0.f) {
			currentPoint = edge.start;
			currentNormal = edge.normalInAtStart;
		} else if (dp >= edge.length) {
			currentPoint = edge.end;
			currentNormal = edge.normalInAtEnd;
		} else {
			currentPoint = edge.start + dp * edge.unitVec;
			currentNormal = edge.normal;
		}
		currentLength = glm::length(P - currentPoint);
		if (currentLength < distanceSoFar) {
			closestPointSoFar = currentPoint + currentNormal * _margin;
			distanceSoFar = currentLength;
		}
	}
	return closestPointSoFar;

}

std::vector<glm::vec2> WalkArea::getPath(glm::vec2 A, glm::vec2 B) {
	auto A1 = closestPointIn(A);
	auto B1 = closestPointIn(B);
	addNode(0, A1);
	addNode(1, B1);
	std::vector<int> p;
	_graph->shortestPath(0, 1, p);
	std::vector<glm::vec2> path;
	for (auto it = p.rbegin(); it != p.rend(); it++) {
		auto p = _graph->getNode(*it);
		path.push_back(p);
	}
	std::cout << "\n";
	_graph->removeNode(0);
	_graph->removeNode(1);
	return path;
}

void WalkArea::postProcess() {
	// apply z
	if (_zFunc) {
		for (const auto& child : m_children) {
			auto pos = child.second->getWorldPosition();
			auto z = _zFunc->evaluate(pos.x, pos.y);
			child.second->setZ(z);
		}
	}

	if (_scaleFunc) {
		for (const auto& child : m_children) {
			auto pos = child.second->getWorldPosition();
			auto z = _scaleFunc->evaluate(pos.x, pos.y);
			child.second->setScale(z);
		}
	}
}

void WalkArea::setZFunction(std::shared_ptr<FuncXY> f) {
	_zFunc = f;
}

void WalkArea::setScaleFunction(std::shared_ptr<FuncXY> f) {
	_scaleFunc = f;
}