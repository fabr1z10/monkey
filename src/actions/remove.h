#pragma once

#include "nodeaction.h"

class RemoveNode : public NodeAction {
public:
    RemoveNode(int id);
    void start() override;
    int process(double) override;
private:
    std::shared_ptr<Node> _nodeToAdd;
};
