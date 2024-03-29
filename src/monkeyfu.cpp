#include "monkeyfu.h"
#include "assetmanager.h"
#include "pyhelper.h"
#include "pycast.h"
#include "runners/collision_engine.h"

py::dict readDataFile(const std::string& path) {
    py::object os = py::module_::import("yaml");
    py::object open = py::module::import("builtins").attr("open");
    py::object dict = os.attr("safe_load")(open(path, "r"));
    return dict;
}


Engine& getEngine() {
    auto& engine = Engine::instance();
    return engine;
}


Clock* getClock() {
    return Engine::instance().getRoom()->getRunner<Clock>();
}

void prova(glm::vec2 p) {
    std::cout << p.x << "\n";


}
glm::vec4 fromHex(const std::string& str) {
	unsigned int r = std::stoul(str.substr(0, 2), nullptr, 16);
	unsigned int g = std::stoul(str.substr(2, 2), nullptr, 16);
	unsigned int b = std::stoul(str.substr(4, 2), nullptr, 16);
	return glm::vec4(r / 255.f, g / 255.f, b / 255.f, 1.f);

}

std::shared_ptr<Model> getTiled(const std::string& id, const pybind11::kwargs& args) {
    auto& am = AssetManager::instance();
    return am.getTiled(id, args);

}

std::shared_ptr<Node> getSprite(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getSprite(id);
}

std::shared_ptr<Model> getSpriteModel(const std::string& id) {
    auto& am = AssetManager::instance();
    return am.getSpriteModel(id);
}

bool checkLOS(glm::vec2 P0, glm::vec2 P1, int mask) {
    auto collisionEngine = Engine::instance().getRoom()->getRunner<ICollisionEngine>();
    auto result = collisionEngine->rayCast(glm::vec3(P0.x, P0.y, 0.f), glm::vec3(P1.x, P1.y, 0.f), mask);
    return !result.collide;

}
//std::shared_ptr<PolyMesh> getPolyMesh(const std::string& id) {
//	auto& am = AssetManager::instance();
//	return am.getPolyMesh(id);
//}
//
//
//std::shared_ptr<MultiNode> getMulti(const std::string& id, const std::string& batch) {
//	auto& am = AssetManager::instance();
//	return am.getMulti(id, batch);
//}
//
Node* getNode(int id) {
	return Engine::instance().getNode(id);
}
//
//std::unordered_set<Node*> getNodes(const std::string& tag) {
//	return Engine::instance().getNodes(tag);
//}
//
//Camera* getCamera(int id) {
//    return Engine::instance().getRoom()->getCamera(id);
//}
//
////IBatch* getBatch(int shader, int batch) {
////    return Engine::instance().getRoom()->getBatch(shader, batch);
////}
//
void closeRoom() {
	Engine::instance().closeRoom();
}
//
int playScript(const std::shared_ptr<Script> s) {
    return Engine::instance().getRoom()->getRunner<Scheduler>()->add(s);
}

int killScript(const std::string& id) {
    Engine::instance().getRoom()->getRunner<Scheduler>()->kill(id);
}
//
