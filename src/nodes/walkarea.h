#pragma once

#include "../node.h"
#include "../shapes/polygon.h"
#include "../math/graph.h"
#include "../math/funcxy.h"

class WalkArea : public Node {
public:
	WalkArea(const pybind11::kwargs& args);
	void onClick(glm::vec2 P, int, int);
	glm::vec2 closestPointIn(glm::vec2 P);
	std::vector<glm::vec2> getPath(glm::vec2 A, glm::vec2 B);
	void postProcess() override;
	void setZFunction(std::shared_ptr<FuncXY> f);
	void setScaleFunction(std::shared_ptr<FuncXY> f);
private:
	struct EdgeData {
		glm::vec2 unitVec;
		glm::vec2 start, end;
		glm::vec2 normalInAtStart;
		glm::vec2 normalInAtEnd;
		glm::vec2 normal;
		float length;
		bool isHole;
	};



	void addPath(const std::vector<glm::vec2>& path, bool isHole, int);
	int addNode(int, glm::vec2 point);
	std::shared_ptr<Polygon> _poly;

	std::shared_ptr<Graph<int, glm::vec2>> _graph;
	std::vector<EdgeData> _edgeData;

	std::shared_ptr<FuncXY> _zFunc;
	std::shared_ptr<FuncXY> _scaleFunc;
	float _margin;
};

//class WalkAreaForwarder : public Node {
//public:
//	WalkAreaForwarder(const pybind11::kwargs &args);
//	void onClick(glm::vec2 P, int, int);
//
//};