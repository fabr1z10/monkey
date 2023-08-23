#pragma once


#include "engine.h"
#include "models/sprite.h"
#include "runners/scheduler.h"
#include "batch.h"
#include "multinode.h"


Engine& getEngine();
std::shared_ptr<Sprite> getSprite(const std::string& id);
std::shared_ptr<MultiNode> getMulti(const std::string& id, const std::string& batch);
Node* getNode(int id);
IBatch* getBatch(int shader, int batch);
Camera* getCamera(int id);

int playScript(const std::shared_ptr<Script> s);
void closeRoom();


