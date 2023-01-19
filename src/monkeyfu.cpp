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
