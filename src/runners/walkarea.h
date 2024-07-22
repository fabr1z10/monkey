#pragma once

#include "../runner.h"
#include "collision_engine.h"
#include "../math/graph.h"

class Baseline;

class WalkArea {
public:
    WalkArea(std::vector<float> &p, float wallThickness);

    // polygon needs to be supplied in CCW!!!
    void addPolyWall(std::vector<float>& points);
    void addLineWall(std::vector<float>& points);
    void addDynamic(Node*);
    std::vector<glm::vec2> findPath(glm::vec2 source, glm::vec2 target);
    glm::vec2 getClosestPointInArea(glm::vec2);
    void recompute();
    float getScale(float x, float y) const;
    std::shared_ptr<Node> getColliderNode();
private:
    struct PolygonInfo {
        PolygonInfo(const std::vector<glm::vec2>& verts);
        std::vector<glm::vec2> vertices;
        std::vector<glm::vec2> unitEdges;
        std::vector<float> lengths;
        std::vector<glm::vec2> normals;

    };

    static std::vector<glm::vec2> vecCvt(const std::vector<float>& p) ;
    void processPoly(const std::vector<glm::vec2>& p, bool isHole, glm::vec2 origin= glm::vec2(0.f, 0.f));
    void processPolyline(const std::vector<Seg>& p, glm::vec2 origin);
    int addNode(glm::vec2 P);
    bool intersectsGeometry(glm::vec2 A, glm::vec2 B);
    void updateClosestPoint(const PolygonInfo& poly, glm::vec2 P, float& bestSoFar, glm::vec2& closest, glm::vec2& normal);
    struct WallInfo {
        WallInfo(glm::vec2 p0, glm::vec2 p1, int node1, int node2) : p0(p0), p1(p1), node1(node1), node2(node2) {}
        glm::vec2 p0;
        glm::vec2 p1;
        int node1;
        int node2;
    };
    void addPolygon(std::vector<float>& points, bool isHole);

    void pippo(Node*);
    //int _flag;
    float _wallThickness;
    std::unique_ptr<Graph> _graph;
    std::vector<WallInfo> _walls;
    std::unique_ptr<PolygonInfo> _walkArea;
    std::vector<std::unique_ptr<PolygonInfo>> _holes;
    std::vector<Node*> _dynamicHoles;
    std::unordered_set<std::pair<int, int>> _adjacentNodes;
    int _currentPoly;
    float _adjust;
    std::vector<std::vector<glm::vec2>> _geometry;
    bool pointInWalkArea(glm::vec2);
};


class WalkManager : public Runner {
public:
    WalkManager(glm::vec2 yBounds);
    void start() override;
    void update(double) override {}
    WalkArea* getArea (int id);
    void addWalkArea(std::shared_ptr<WalkArea> area);
    void addBaseLine(Baseline*);
    void rmBaseline(Baseline*);
    void recomputeBaselines();
    float getZ(float x, float y) const;

private:
    ICollisionEngine* _collisionEngine;
    std::vector<std::shared_ptr<WalkArea>> _walkAreas;
    std::unordered_set<Baseline*> _baselines;
    float _za, _zb;

};

inline WalkArea * WalkManager::getArea(int id) {
    return _walkAreas[id].get();
}

inline void WalkManager::addWalkArea(std::shared_ptr<WalkArea> area) {
    _walkAreas.push_back(area);

}

