#pragma once


#include "engine.h"
#include "models/sprite.h"
#include "models/polymesh.h"
#include "runners/scheduler.h"
#include "batch.h"
#include "multinode.h"

void readDataFile(const std::string& id, const std::string& path);
Engine& getEngine();
std::shared_ptr<Sprite> getSprite(const std::string& id);
std::shared_ptr<PolyMesh> getPolyMesh(const std::string& id);
glm::vec4 fromHex(const std::string&);
std::shared_ptr<MultiNode> getMulti(const std::string& id, const std::string& batch);
Node* getNode(int id);
std::unordered_set<Node*> getNodes(const std::string& tag);
IBatch* getBatch(int shader, int batch);
Camera* getCamera(int id);

int playScript(const std::shared_ptr<Script> s);
void closeRoom();


