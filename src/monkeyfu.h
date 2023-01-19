#pragma once


#include "engine.h"
#include "models/sprite.h"


Engine& getEngine();
std::shared_ptr<Sprite> getSprite(const std::string& id);