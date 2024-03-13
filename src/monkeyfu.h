#pragma once


#include "engine.h"
#include "pycast.h"
#include "models/sprite.h"
//#include "models/polymesh.h"
#include "runners/scheduler.h"
#include "runners/clock.h"

//#include "batch.h"
//#include "multinode.h"
void prova(glm::vec2 c);
py::dict readDataFile(const std::string& path);
Engine& getEngine();

std::shared_ptr<Model> getTiled(const std::string& id, const pybind11::kwargs&);
std::shared_ptr<Node> getSprite(const std::string& id);
std::shared_ptr<Model> getSpriteModel(const std::string& id);

bool checkLOS(glm::vec2 P0, glm::vec2 P1, int mask = 0);

Clock* getClock();
//std::shared_ptr<PolyMesh> getPolyMesh(const std::string& id);
glm::vec4 fromHex(const std::string&);
//std::shared_ptr<MultiNode> getMulti(const std::string& id, const std::string& batch);
Node* getNode(int id);
//std::unordered_set<Node*> getNodes(const std::string& tag);
//IBatch* getBatch(int shader, int batch);
//Camera* getCamera(int id);
//
int playScript(const std::shared_ptr<Script> s);
void closeRoom();


