#pragma once

#include "../runner.h"
#include "collision_engine.h"
#include "../math/graph.h"

class WalkArea : public Runner {
public:
    WalkArea(std::vector<float> &p, float wallThickness);

    // polygon needs to be supplied in CCW!!!
    void addPolyWall(std::vector<float>& points);
    void addLineWall(std::vector<float>& points);
    void findPath(glm::vec2 source, glm::vec2 target);
    void start() override;
    void update(double) override {}
    glm::vec2 getClosestPointInArea(glm::vec2);
private:
    int addNode(glm::vec2 P);
    bool intersectsGeometry(glm::vec2 A, glm::vec2 B);
    void updateClosestPoint(std::vector<glm::vec2>& path, glm::vec2 P, float& bestSoFar, glm::vec2& closest, glm::vec2& normal);
    struct WallInfo {
        WallInfo(glm::vec2 p0, glm::vec2 p1, int node1, int node2) : p0(p0), p1(p1), node1(node1), node2(node2) {}
        glm::vec2 p0;
        glm::vec2 p1;
        int node1;
        int node2;
    };
    void addPolygon(std::vector<float>& points, bool isHole);

    void pippo(Node*);
    ICollisionEngine* _collisionEngine;
    //int _flag;
    float _wallThickness;
    std::unique_ptr<Graph> _graph;
    std::vector<WallInfo> _walls;
    std::vector<glm::vec2> _walkArea;
    std::vector<std::vector<glm::vec2>> _holes;
    std::unordered_set<std::pair<int, int>> _adjacentNodes;
    int _currentPoly;
    float _adjust;
};
