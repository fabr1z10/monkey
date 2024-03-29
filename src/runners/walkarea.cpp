#include "walkarea.h"

#include "../engine.h"
#include <iostream>
#include "../util.h"
#include "../shapes/polygon.h"
#include "../shapes/polyline.h"

// define a walkarea with a polygon
WalkArea::WalkArea(std::vector<float> &p, float wallThickness) : Runner(), _wallThickness(wallThickness), _currentPoly(0) {
    _graph = std::make_unique<Graph>();
    _adjust = 1.f;

    //addPolygon(p, false);
    _geometry.push_back(vecCvt(p));
}

std::vector<glm::vec2> WalkArea::vecCvt(const std::vector<float>& p) {
    std::vector<glm::vec2> pts;
    for (size_t i = 0; i < p.size(); i+= 2) {
        pts.emplace_back(p[i], p[i+1]);
    }
    return pts;
}

int WalkArea::addNode(glm::vec2 P) {

    int newNodeId = _graph->addNode(GraphNode{P});
    int n = _graph->getNodeCount();
    for (int i = 0; i < n; i++) {
        if (i == newNodeId) continue;
        const auto& node = _graph->getNode(i);
        glm::vec2 midPoint = (node.pos + P) * 0.5f;


        if (pointInWalkArea(midPoint) && !intersectsGeometry(node.pos, P)) {
            _graph->addEdge(newNodeId, i);
        }
    }
    return newNodeId;

}

bool WalkArea::intersectsGeometry(glm::vec2 A, glm::vec2 B) {

    for (auto& w : _walls) {
        if (seg2segStrict(A, B, w.p0, w.p1)) {
            return true;
        }
    }
    return false;

}

void WalkArea::processPolyline(const std::vector<Seg> &p, glm::vec2 origin) {

    glm::vec2 previousNormal(0.f);
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> normals;
    std::vector<glm::vec2> above;
    std::vector<glm::vec2> below;

    points.push_back(p[0].P0 + origin);
    normals.push_back(p[0].n);
    for (size_t i = 0; i< p.size()-1; ++i) {
        glm::vec2 n = (p[i].n + p[i+1].n) * 0.5f;
        points.push_back(p[i].P1 + origin);
        normals.push_back(n);

    }
    points.push_back(p.back().P1 + origin);
    normals.push_back(p.back().n);

    for (size_t i = 0; i < points.size(); ++i) {
        below.push_back(points[i] - normals[i] * _wallThickness);
        above.push_back(points[i] + normals[i] * _wallThickness);
    }
    below.insert(below.end(), above.rbegin(), above.rend());
    processPoly(below, true);
}

bool WalkArea::pointInWalkArea(glm::vec2 P) {
    if (!pnpoly(_walkArea, P)) {
        return false;
    }
    for (const auto& hole : _holes) {
        if (pnpoly(hole, P)) {
            return false;
        }
    }
    return true;
}

void WalkArea::processPoly(const std::vector<glm::vec2> &p, bool isHole, glm::vec2 origin) {
    int l = p.size();
    auto p0 = p.back() + origin;
    // check every internal angle. If angle is convex (< 180) --> add point if hole,
    // if angle is concave --> add point if walkarea
    int previousPointNodeId = -1;
    std::vector<glm::vec2> pp;
    int currentPointNodeId = -1;
    int originNodeId = -1;
    for (int i = 0; i < p.size(); i++) {
        currentPointNodeId = -1;
        int j = (i + 1) % l;
        auto p1 = p[i] + origin;
        auto p2 = p[j] + origin;
        // check angle
        bool isConvex = cross2D(p1 - p0, p2 -p1) > 0;
        if (isConvex == isHole) {
            currentPointNodeId =  _graph->addNode(GraphNode{p1});
            if (i == 0) originNodeId = currentPointNodeId;
            //_nodeToPoly[currentPointNodeId] = _currentPoly;
        }
        _walls.emplace_back(WallInfo(p0, p1,previousPointNodeId,currentPointNodeId));
        if (previousPointNodeId != -1 && currentPointNodeId != -1) {
            std::cout << "ADJACENT: " << previousPointNodeId << ", " << currentPointNodeId << "\n";
            _adjacentNodes.insert(std::make_pair(previousPointNodeId, currentPointNodeId));
            _adjacentNodes.insert(std::make_pair(currentPointNodeId, previousPointNodeId));
        }
        previousPointNodeId = currentPointNodeId;
        pp.push_back(p0);
        pp.push_back(p1);
        p0 = p1;

    }
    if (originNodeId != -1 && currentPointNodeId != -1) {
        std::cout << "ADJACENT: " << originNodeId << ", " << currentPointNodeId << "\n";
        _adjacentNodes.insert(std::make_pair(originNodeId, currentPointNodeId));
        _adjacentNodes.insert(std::make_pair(currentPointNodeId, originNodeId));
    }
    if (isHole) {
        _holes.push_back(p);
    } else {
        _walkArea = p;
    }
    _currentPoly++;
}
void WalkArea::recompute() {

    _graph->clear();
    _walkArea.clear();
    _holes.clear();
    _walls.clear();
    for (size_t i = 0; i < _geometry.size(); ++i) {
        processPoly(_geometry[i], i > 0);
    }

    for (const auto& n : _dynamicHoles) {
        auto shape = n->getComponent<Collider>()->getShape();
        if (shape->getShapeType() == ShapeType::POLYGON) {
            auto poly = std::dynamic_pointer_cast<Polygon>(shape);
            if (poly != nullptr) {
                auto outline = poly->getOutline();
                auto pos = n->getWorldPosition();
                processPoly(outline, true, glm::vec2(pos));
            }
        } else if (shape->getShapeType() == ShapeType::POLYLINE) {
            auto poly = std::dynamic_pointer_cast<PolyLine>(shape);
            auto* segs = poly->getSegments();
            auto pos = n->getWorldPosition();
            processPolyline(*segs, glm::vec2(pos));
        }
    }
    // add edges to graph
    int n = _graph->getNodeCount();
    for (int i = 0; i < n; i++) {
        for (int j=i+1; j <n; j++) {
            const auto& ni= _graph->getNode(i);
            const auto& nj= _graph->getNode(j);
            std::cout << " checking node " << i << " vs node " << j << "\n";
            std::cout << "(" << ni.pos.x << ", " << ni.pos.y << ") --> (" << nj.pos.x << ", " << nj.pos.y << ")\n";

            float t = 0.f;
            bool addEdge = true;
            for (auto& w : _walls) {
                if (w.node1 == i || w.node2 == i || w.node1 == j || w.node2 == j) {
                    continue;
                }

                if (seg2segStrict(ni.pos, nj.pos, w.p0, w.p1)) {
                    addEdge = false;
                    break;
                }


            }
            // check mid point is internal to walkarea and external to holes
            // unless edge is between consecutive points
            if (addEdge && _adjacentNodes.count(std::make_pair(i, j)) == 0) {

                glm::vec2 midPoint = 0.5f * (ni.pos + nj.pos);
                bool isInWalkArea = pnpoly(_walkArea, midPoint);
                if (!isInWalkArea) {
                    addEdge = false;
                } else {
                    for (const auto &h : _holes) {
                        if (pnpoly(h, midPoint)) {
                            addEdge = false;
                            break;
                        }
                    }
                }

            }
            if (addEdge) {
                std::cout << "ADDED EDGE: (" << ni.pos.x << ", " << ni.pos.y << ") --> (" << nj.pos.x << ", " << nj.pos.y << ")\n";
                _graph->addEdge(i,j);
            }

        }
    }
}

void WalkArea::start() {
    // we need all colliders with
    _collisionEngine = Engine::instance().getRoom()->getRunner<ICollisionEngine>();
    recompute();



}

void WalkArea::addPolygon(std::vector<float> &p, bool isHole) {
    _geometry.push_back(vecCvt(p));

}

void WalkArea::addDynamic(Node * n) {
    _dynamicHoles.push_back(n);
    n->onRemove.reg([&] (Node* m) {
        _dynamicHoles.erase(std::remove(_dynamicHoles.begin(),_dynamicHoles.end(), m));
    });
}

void WalkArea::addPolyWall(std::vector<float> &points) {
    addPolygon(points, true);
}

void WalkArea::addLineWall(std::vector<float> &p) {
    // we need first to create a polygon out of the contour
    glm::vec2 u0(0.f);
    std::vector<glm::vec2> pointsBelow;
    std::vector<glm::vec2> pointsAbove;
    glm::vec2 p1;
    glm::vec2 n;
    for (int i = 0; i < p.size(); i+=2) {
        p1 = glm::vec2(p[i], p[i+1]);
        if (i < p.size() - 2) {
            glm::vec2 p2(p[i+2], p[i+3]);
            glm::vec2 u1 = glm::normalize(p2 - p1);
            n = (u0 == glm::vec2(0.f)) ?  rot90(u1) : (0.5f * (rot90(u0) + rot90(u1)));
            u0 = u1;
        } else {
            n = rot90(u0);
        }
        // rotate counterclockwise
        glm::vec2 above = p1 + n * _wallThickness;
        glm::vec2 below = p1 - n * _wallThickness;
        pointsBelow.push_back(below);
        pointsAbove.push_back(above);
    }

    std::vector<float> points;
    for (int i = 0; i < pointsBelow.size(); i++) {
        points.push_back(pointsBelow[i].x);
        points.push_back(pointsBelow[i].y);
    }
    for (int i = pointsAbove.size()-1; i >= 0; i--) {
        points.push_back(pointsAbove[i].x);
        points.push_back(pointsAbove[i].y);
    }
    addPolygon(points, true);

}

void WalkArea::updateClosestPoint(std::vector<glm::vec2>& path, glm::vec2 P, float& bestSoFar, glm::vec2& incumbent, glm::vec2& normal) {
    for (size_t i = 0; i < path.size(); ++i) {
        glm::vec2 A(path[i]);
        glm::vec2 B(path[(i + 1) % path.size()]);
        glm::vec2 u = glm::normalize(B - A);
        float l = glm::length(B - A);
        auto d = glm::dot(P - A, u);
        glm::vec2 closest;
        if (d < 0) {
            closest = A;
        } else if (d > l) {
            closest = B;
        } else {
            closest = A + d * u;
        }
        float currentDistance = glm::distance(closest, P);
        if (currentDistance < bestSoFar) {
            bestSoFar = currentDistance;
            incumbent = closest;
            normal = rot90(u);
        }
    }
}

glm::vec2 WalkArea::getClosestPointInArea(glm::vec2 P) {
    if (pnpoly(_walkArea, P)) {
        // check is not inside holes
        for (auto& hole : _holes) {
            if (pnpoly(hole, P)) {
                glm::vec2 best;
                float bestDistance = std::numeric_limits<float>::infinity();
                glm::vec2 normal;
                updateClosestPoint(hole, P, bestDistance, best, normal);
                return best - normal * _adjust;
            }
        }
        return P;
    } else {
        glm::vec2 best;
        float bestDistance = std::numeric_limits<float>::infinity();
        glm::vec2 normal;
        updateClosestPoint(_walkArea, P, bestDistance, best, normal);
        return best + normal * _adjust;


    }
}

std::vector<glm::vec2> WalkArea::findPath(glm::vec2 source, glm::vec2 target) {
    // first, check that both source and target are inside the valid area
    glm::vec2 rs = getClosestPointInArea(source);
    glm::vec2 rt = getClosestPointInArea(target);

    // then add source + target nodes to the graph --> connect with other nodes
    int sourceId = addNode(rs);
    int targetId = addNode(rt);

    // find path
    auto path = _graph->findPath(sourceId, targetId);

    // remove nodes
    _graph->rmNode(targetId);
    _graph->rmNode(sourceId);

    return path;
}