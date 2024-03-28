#include <list>
#include "graph.h"

int Graph::addNode(GraphNode node) {
    int index = _nextNodeId++;
    _nodes[index] = node;
    return index;
}

void Graph::addEdge(int from, int to) {
    _edges[from].insert(to);
    _edges[to].insert(from);
}

void Graph::rmNode(int id) {
    for (int i = id; i < _nodes.size(); ++i) {
        _nodes.erase(i);
        _edges.erase(i);
        for (auto &a : _edges) {
            a.second.erase(i);
        }
    }
    _nextNodeId=id;
}

void Graph::clear() {
    _nodes.clear();
    _edges.clear();
    _nextNodeId = 0;
}

const GraphNode & Graph::getNode(int index) const {
    return _nodes.at(index);

}

std::vector<glm::vec2> Graph::findPath(int source, int target) const {
    // keeps list of nodes to explore
    std::list<std::pair<int, int>> l{ std::make_pair(source, 0)};
    std::unordered_map<int, std::pair<int, int>> reachableNodes;
    reachableNodes[source] = {0, -1};

    while (!l.empty()) {
        auto current = l.front();
        l.pop_front();
        auto it = _edges.find(current.first);
        if (it != _edges.end()) {
            for (const auto &e : it->second) {
                if (reachableNodes.count(e) == 0 || reachableNodes.at(e).first > current.second + 1) {
                    reachableNodes[e] = std::make_pair(current.second + 1, current.first);
                    if (e != target) {
                        l.emplace_back(e, current.second + 1);
                    }
                }
            }
        }
    };

    // now check if target is reachable
    if (reachableNodes.count(target) > 0) {
        std::cout << "target is reachable\n";
        std::vector<glm::vec2> path;
        int curr = target;
        while (curr != -1) {
            path.push_back(_nodes.at(curr).pos);
            curr = reachableNodes.at(curr).second;
        }
        return path;
    }

    return {};

}
