#pragma once


#include "engine.h"
#include "models/sprite.h"
#include "runners/scheduler.h"


Engine& getEngine();
std::shared_ptr<Sprite> getSprite(std::shared_ptr<IBatch> batch, const std::string& id);
std::shared_ptr<Node> getNode(int id);
int playScript(const std::shared_ptr<Script> s);
void closeRoom();


