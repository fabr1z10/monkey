#pragma once


#include "engine.h"
#include "models/sprite.h"
#include "runners/scheduler.h"
#include "batch.h"


Engine& getEngine();
std::shared_ptr<Sprite> getSprite(const std::string& id);
std::shared_ptr<Node> getNode(int id);
IBatch* getBatch(int shader, int batch);
Camera* getCamera(const std::string& id);

int playScript(const std::shared_ptr<Script> s);
void closeRoom();


