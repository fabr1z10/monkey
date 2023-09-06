#pragma once

#include "../node.h"
#include "../shapes/polygon.h"
#include "../math/graph.h"
#include "../math/funcxy.h"

struct EdgeData {
	glm::vec2 unitVec;
	int istart, iend;
	glm::vec2 start, end;
	glm::vec2 normalInAtStart;
	glm::vec2 normalInAtEnd;
	glm::vec2 normal;
	float length;
	bool isHole;
};

class WalkArea : public Node {
public:
	struct ClosestPointResult {

		glm::vec2 closestPoint;
		glm::vec2 normalInward;
		int edgeId;
		int nodeIndex;
	};

	explicit WalkArea(const pybind11::kwargs& args);
	virtual ClosestPointResult closestPointIn(glm::vec2 P) const;
	virtual std::vector<glm::vec2> getPath(glm::vec2 A, glm::vec2 B) = 0;
	void postProcess() override;
	void setZFunction(std::shared_ptr<FuncXY> f);
	void setScaleFunction(std::shared_ptr<FuncXY> f);
	virtual int addNode(int newNodeId, glm::vec2 point, int edgeId) = 0;
protected:
	std::shared_ptr<FuncXY> _zFunc;
	std::shared_ptr<FuncXY> _scaleFunc;

	std::shared_ptr<Graph<int, glm::vec2>> _graph;
	std::vector<EdgeData> _edgeData;
};


class WalkAreaPolygon : public WalkArea {
public:
	explicit WalkAreaPolygon(const pybind11::kwargs& args);
	//void onClick(glm::vec2 P, int, int);
	ClosestPointResult closestPointIn(glm::vec2 P) const override;
	int addNode(int, glm::vec2 point, int edgeId) override;
	std::vector<glm::vec2> getPath(glm::vec2 A, glm::vec2 B) override;
private:




	void addPath(const std::vector<glm::vec2>& path, bool isHole, int);


	std::shared_ptr<Polygon> _poly;




	float _margin;
};


class WalkAreaPolyline : public WalkArea {
public:
	explicit WalkAreaPolyline(const pybind11::kwargs& args);
	std::vector<glm::vec2> getPath(glm::vec2 A, glm::vec2 B) override;
	int addNode(int, glm::vec2 point, int edgeId) override;
private:
	std::unordered_map<int, std::pair<int, int>> _edgeEndPoints;
};

//class WalkAreaForwarder : public Node {
//public:
//	WalkAreaForwarder(const pybind11::kwargs &args);
//	void onClick(glm::vec2 P, int, int);
//
//};