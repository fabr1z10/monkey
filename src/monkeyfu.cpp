#include "monkeyfu.h"
#include "assetmanager.h"

Engine& getEngine() {
    auto& engine = Engine::instance();
    return engine;
}

std::shared_ptr<Sprite> getSprite(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getSprite(id);
}

std::shared_ptr<Node> getNode(int id) {
	return Engine::instance().getNode(id);
}

Camera* getCamera(const std::string& id) {
    return Engine::instance().getRoom()->getCamera(id);
}

IBatch* getBatch(int shader, int batch) {
    return Engine::instance().getRoom()->getBatch(shader, batch);
}

void closeRoom() {
	Engine::instance().closeRoom();
}

int playScript(const std::shared_ptr<Script> s) {
	return Engine::instance().getRoom()->getRunner<Scheduler>()->add(s);
}

