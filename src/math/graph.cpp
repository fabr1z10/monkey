#include "graph.h"

int Graph::addNode(GraphNode node) {
    int index = _nextNodeId++;
    _nodes[index] = node;
    return index;
}

void Graph::addEdge(int from, int to) {
    _edges[from].insert(to);
}

void Graph::rmNode(int id) {
    _nodes.erase(id);
    _edges.erase(id);
    for (auto& a : _edges) {
        a.second.erase(id);
    }
}

const GraphNode & Graph::getNode(int index) const {
    return _nodes.at(index);

}

std::vector<int> Graph::findPath(int source, int target) const {

}
