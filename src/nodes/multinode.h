#pragma once

#include "../node.h"


class MultiNode : public Node
{
public:
    MultiNode() : Node() {}
    void update(double) override;
    void addSubNode(std::shared_ptr<Node>);
    void select(int);
private:
    std::vector<std::shared_ptr<Node>> _nodes;
    int _current;
};