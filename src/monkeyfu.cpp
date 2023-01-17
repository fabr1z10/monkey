#include "monkeyfu.h"


Engine& getEngine() {
    auto& engine = Engine::instance();
    return engine;
}