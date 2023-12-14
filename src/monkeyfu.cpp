#include "monkeyfu.h"
#include "assetmanager.h"


void readDataFile(const std::string& id, const std::string& path) {
    py::object os = py::module_::import("yaml");
    py::object open = py::module::import("builtins").attr("open");

    py::object dict = os.attr("safe_load")(open(path, "r"));
    Engine::instance().getConfig().attr(id.c_str()) = dict;
}

Engine& getEngine() {
    auto& engine = Engine::instance();
    return engine;
}

glm::vec4 fromHex(const std::string& str) {
	unsigned int r = std::stoul(str.substr(0, 2), nullptr, 16);
	unsigned int g = std::stoul(str.substr(2, 2), nullptr, 16);
	unsigned int b = std::stoul(str.substr(4, 2), nullptr, 16);
	return glm::vec4(r / 255.f, g / 255.f, b / 255.f, 1.f);

}

std::shared_ptr<Sprite> getSprite(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getSprite(id);
}
std::shared_ptr<PolyMesh> getPolyMesh(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getPolyMesh(id);
}


std::shared_ptr<MultiNode> getMulti(const std::string& id, const std::string& batch) {
	auto& am = AssetManager::instance();
	return am.getMulti(id, batch);
}

Node* getNode(int id) {
	return Engine::instance().getNode(id);
}

std::unordered_set<Node*> getNodes(const std::string& tag) {
	return Engine::instance().getNodes(tag);
}

Camera* getCamera(int id) {
    return Engine::instance().getRoom()->getCamera(id);
}

//IBatch* getBatch(int shader, int batch) {
//    return Engine::instance().getRoom()->getBatch(shader, batch);
//}

void closeRoom() {
	Engine::instance().closeRoom();
}

int playScript(const std::shared_ptr<Script> s) {
	return Engine::instance().getRoom()->getRunner<Scheduler>()->add(s);
}

