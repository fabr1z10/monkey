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
#include "../models/lines.h"

using namespace pybind11::literals; // to bring in the `_a` literal

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

WalkArea::WalkArea(const pybind11::kwargs &args) : Node(), _zFunc(nullptr), _scaleFunc(nullptr) {

	_graph = std::make_shared<Graph<int, glm::vec2>>();

	auto zFunc = py_get_dict<std::vector<float>>(args, "z_func", std::vector<float>());
	auto scaleFunc = py_get_dict<std::vector<float>>(args, "scale_func", std::vector<float>());
	if (!zFunc.empty()) {
		setZFunction(std::make_shared<PiecewiseLinearYFunc>(zFunc));
	}
	if (!scaleFunc.empty()) {
		setScaleFunction(std::make_shared<PiecewiseLinearYFunc>(scaleFunc));
	}

}

WalkAreaPolyline::WalkAreaPolyline(const pybind11::kwargs &args) : WalkArea(args) {
	auto nodes = args["nodes"].cast<pybind11::dict>();
	for (const auto& node : nodes) {
		auto id = node.first.cast<int>();
		auto point = node.second.cast<glm::vec2>();
		_graph->addNode(id, point);
	}
	auto edges = args["edges"].cast<std::vector<int>>();
	int edgeId = 0;
	for (size_t i = 0; i < edges.size(); i+=2) {
		glm::vec2 A = _graph->getNode(edges[i]);
		glm::vec2 B = _graph->getNode(edges[i+1]);
		auto dist = glm::distance(A, B);
		_graph->addEdge(edges[i], edges[i+1], dist);
		EdgeData edge;
		edge.istart = edges[i];
		edge.iend = edges[i+1];
		edge.start = A;
		edge.end = B;
		edge.unitVec = glm::normalize(B - A);
		edge.normal = rot90(edge.unitVec, false);
		edge.length = dist;
		_edgeData.push_back(edge);
		_edgeEndPoints[edgeId] = {edges[i], edges[i+1]};
		edgeId++;
	}
	auto batchId = py_get_dict<std::string>(args, "batch", "");
	if (!batchId.empty()) {
		std::vector<float> rawData;
		for (const auto& edge : _edgeData) {
			rawData.insert(rawData.end(), { edge.start.x, edge.start.y, 0.f, edge.end.x, edge.end.y, 0.f});
		}
		auto model = std::make_shared<LineModel>(pybind11::dict("color"_a=glm::vec4(1.f), "points"_a=rawData));
		Node::setModel(model, pybind11::dict("batch"_a = batchId));
	}

}

void WalkAreaPolygon::addEdges(const std::vector<glm::vec2> & outline, bool isHole) {
	glm::vec2 uvec = glm::normalize(outline.front() - outline.back());
	for (size_t i = 0; i < outline.size(); ++i) {
		EdgeData edge;
		glm::vec2 A = outline[i];
		glm::vec2 B = outline[(i+1) % outline.size()];
		edge.start = A;
		edge.end = B;
		edge.istart =-1;
		edge.iend =-1;
		edge.unitVec = glm::normalize(B-A);
		edge.normal = rot90(edge.unitVec, isHole);
		edge.length = glm::length(B-A);
		edge.normalInAtStart = glm::normalize(edge.unitVec - uvec);
		if (i > 0)
			_edgeData.back().normalInAtEnd = edge.normalInAtStart;
		uvec = edge.unitVec;
		_edgeData.push_back(edge);
	}
}

WalkAreaPolygon::WalkAreaPolygon(const pybind11::kwargs &args) : WalkArea(args) {
	_margin = py_get_dict<float>(args, "margin", 0.1f);



	// REQUIREMENT:
	// polygon outline must be provided ccw
	// holes cw order
	auto polygon = py_get_dict<std::vector<float>>(args, "poly");
	assert(polygon.size() % 2 == 0);



	_poly = std::make_shared<Polygon>(polygon);
	if (args["holes"] && !args["holes"].is_none()) {
		for (const auto& hole : args["holes"]) {
			_poly->addHole(hole.cast<std::vector<float>>());
		}
	}

	if (args["walls"] && !args["walls"].is_none()) {
		for (const auto& wall : args["walls"]) {
			Wall w;
			w.A = py_get_dict<glm::vec2>(wall, "A");
			w.B = py_get_dict<glm::vec2>(wall, "B");
			w.active = py_get_dict<bool>(wall, "active", true);
			_walls.push_back(w);
		}

	}

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

	createGraph();




}

void WalkAreaPolygon::createGraph() {
	_graph->clear();
	_edgeData.clear();
	int offset = 10;
	const auto& outline = _poly->getOutline();
	addPath(outline, false, offset);
	offset += outline.size();
	addEdges(outline, false);
	int holeCount = _poly->getHoleCount();
	for (size_t i =0 ; i< holeCount; ++i) {
		const auto& holeOutline = _poly->getHoleOutline(i);
		addPath(holeOutline, true, offset);
		offset += holeOutline.size();
		addEdges(holeOutline, true);
	}

}

//void WalkArea::onClick(glm::vec2 P, int button, int action) {
//	std::cout << " clicked walkarea at " << P.x << ", " << P.y << ", " << button << ", " << action << "\n";
////
////	// check if player is on this walkarea
////	const auto& player = Engine::instance().getNodes("player");
////	if (player.empty() || player.size() > 2) {
////		GLIB_FAIL("There must be one and only one item tagged <player>!");
////	}
////	auto node = (*player.begin());
////	if (node->getParent() == this) {
////		std::cout << "ok\n";
//		// crea lo script
//		//
//	auto script = std::make_shared<Script>( pybind11::dict("id"_a = "_player"));
//
//	script->add(
//			std::make_shared<Walk>(pybind11::dict("tag"_a = "player", "target"_a = P)),
//			pybind11::dict());
//	auto scheduler = Engine::instance().getRoom()->getRunner<Scheduler>();
//	scheduler->add(script);
//
////
////		//node->setPosition(P.x, P.y, 0.f);
////	} else {
////		std::cout << "ko\n";
////	}
//
//}


// adds nodes to the graph:
// - Every ‘concave vertex’ (a vertex with an interior angle greater than 180°, so an inward pointing vertex) is a node
// - For holes, all convex vertices in the inner polygon are nodes in the graph instead of the concave vertices.
void WalkAreaPolygon::addPath(const std::vector<glm::vec2>& path, bool isHole, int offset) {
	// consider special case of path made of only 2 points (segment)

	size_t ip = path.size() - 1;
	size_t ic = 0;			// index to current node
	size_t in = 1;			// index to next node
	do {
		glm::vec2 edgeNext = path[in] - path[ic];
		glm::vec2 edgePrev = path[ic] - path[ip];
		float flip = isHole ? -1.0f : 1.0f;
		// cross product is proportional to sin(theta), so theta > 180 (concave vertex) means f < 0
		auto f = cross2D(edgePrev, edgeNext) * flip;
		if (f < 0) {
			glm::vec2 u = glm::normalize(edgeNext);
			glm::vec2 v = glm::normalize(-edgePrev);
			glm::vec2 w = glm::normalize(u+v);
			addNode(offset, path[ic] - w * _margin, -1);
			offset++;
		}
		ip = ic;
		ic = in;
		in = (in + 1) % path.size();

	} while (ic != 0);
}

bool WalkAreaPolygon::intersectWalls(glm::vec2 A, glm::vec2 B) {
	float t{0.f};
	for (const auto& wall : _walls) {
		if (wall.active) {
			seg2seg(A, B, wall.A, wall.B, t);
			if (t >= 0 && t <= 1) {
				return true;
			}
		}
	}
	return false;
}


int WalkAreaPolygon::addNode(int id, glm::vec2 point, int edgeId) {
	// adds a node to the graph
	// and an edge from the new node to every other node that is in line of sight with it
	//int id = m_graph->getVertexCount();

	_graph->addNode(id, point);
	std::cout << "Added node " << id << " at " << point.x << ", " << point.y << "\n";
	float t{0.f};
	for (const auto& node : _graph->getNodes()) {
		if (node.first != id) {
			glm::vec2 P = node.second;
			if (!_poly->intersectSegment(point, P, t) && !intersectWalls(point, P)) {
				_graph->addEdge(id, node.first, glm::length(point-P));
				std::cout << "Added edge from node " << id << " to node " << node.first << "\n";
			}
		}
	}
	return id;
}

int WalkAreaPolyline::addNode(int id, glm::vec2 point, int edgeId) {
	_graph->addNode(id, point);
	auto endPoints = _edgeEndPoints.at(edgeId);
	//float distance = glm::length(point - )
	_graph->addEdge(id, endPoints.first, glm::length(point - _graph->getNode(endPoints.first)));
	_graph->addEdge(id, endPoints.second, glm::length(point - _graph->getNode(endPoints.second)));
	// adding two edges only, one for each segment
	// we need to recover the end points id for the edge

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

WalkArea::ClosestPointResult WalkAreaPolygon::closestPointIn(glm::vec2 P) const {
	glm::vec3 p(P.x, P.y, 0.f);
	if (_poly->isInside(p)) {
		return ClosestPointResult{P, glm::vec2(0.f), -1};
	}
	// adjust inside
	auto result =WalkArea::closestPointIn(P);
	result.closestPoint += _margin * result.normalInward;
	return result;
}

WalkArea::ClosestPointResult WalkArea::closestPointIn(glm::vec2 P) const {
	ClosestPointResult result;
	float incumbentDistance = std::numeric_limits<float>::infinity();
	size_t edgeId = 0;
	for (const auto& edge : _edgeData) {
		// projection on the line AB
		auto projection = glm::dot(P - edge.start, edge.unitVec);
		glm::vec2 currentClosestPoint;
		glm::vec2 currentNormal;
		int nodeId {-1};
		if (projection <= 0.f) {
			currentClosestPoint = edge.start;
			currentNormal = edge.normalInAtStart;
			nodeId = edge.istart;
		} else if (projection >= edge.length) {
			currentClosestPoint = edge.end;
			currentNormal = edge.normalInAtEnd;
			nodeId = edge.iend;
		} else {
			currentClosestPoint = edge.start + projection * edge.unitVec;
			currentNormal = edge.normal;
		}
		float currentLength = glm::distance(currentClosestPoint, P);
		if (currentLength < incumbentDistance) {
			incumbentDistance = currentLength;
			result.closestPoint = currentClosestPoint;
			result.normalInward = currentNormal;
			result.edgeId = edgeId;
			result.nodeIndex = nodeId;
		}
		edgeId++;
	}
	return result;
}

std::vector<glm::vec2> WalkAreaPolygon::getPath(glm::vec2 A, glm::vec2 B) {
	auto A1 = closestPointIn(A);
	auto B1 = closestPointIn(B);
	// 0 marks the start point, 1 marks the end point
	addNode(0, A1.closestPoint, A1.edgeId);
	addNode(1, B1.closestPoint, B1.edgeId);



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

std::vector<glm::vec2> WalkAreaPolyline::getPath(glm::vec2 A, glm::vec2 B) {
	auto A1 = closestPointIn(A);
	auto B1 = closestPointIn(B);
	// 0 marks the start point, 1 marks the end point
	if (A1.edgeId == B1.edgeId) {
		if (A1.nodeIndex == B1.nodeIndex && A1.nodeIndex != -1) {
			return {};
		} else {
			return {A1.closestPoint, B1.closestPoint};
		}
	}
	int startIndex = 0;
	int endIndex = 1;
	if (A1.nodeIndex != -1) {
		startIndex = A1.nodeIndex;
	} else {
		addNode(0, A1.closestPoint, A1.edgeId);
	}
	if (B1.nodeIndex != -1) {
		endIndex = B1.nodeIndex;
	} else {
		addNode(1, B1.closestPoint, B1.edgeId);
	}
	std::vector<int> p;
	_graph->shortestPath(startIndex, endIndex, p);
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

