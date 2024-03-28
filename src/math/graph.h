#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

struct GraphNode {
    glm::vec2 pos;
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