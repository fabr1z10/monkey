#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

struct GraphNode {
    GraphNode(glm::vec2 position, int polyIndex, int vertexIndex, bool convex, bool hole, glm::vec2 prev, glm::vec2 next)
        : pos(position), polygonIndex(polyIndex), vertexIndex(vertexIndex), convex(convex), hole(hole), previous(prev), next(next) {}

    GraphNode(glm::vec2 position) : pos(position), polygonIndex(-1), vertexIndex(-1), convex(false), hole(false) {}

    glm::vec2 pos;
    int polygonIndex;
    int vertexIndex;
    bool convex;
    bool hole;
    glm::vec2 previous;
    glm::vec2 next;
    bool test(glm::vec2 P) const;
};

class Graph {

public:
    Graph() = default;

    int addNode(GraphNode node);

    void addEdge(int from, int to);

    int getNodeCount() const;

    const GraphNode& getNode(int index) const;

    std::vector<glm::vec2> findPath(int source, int target) const;

    void rmNode(int id);

    void clear();
private:
    int _nextNodeId;

    std::unordered_map<int, GraphNode> _nodes;
    std::unordered_map<int, std::unordered_set<int>> _edges;
};

inline int Graph::getNodeCount() const {
    return _nodes.size();
}