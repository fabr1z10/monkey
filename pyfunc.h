#pragma once

#include "models/sprite.h"
#include "shape.h"
#include "models/tiled.h"

std::shared_ptr<Sprite> getSprite(const std::string&);
std::shared_ptr<TiledModel> getTiled(const std::string&);

std::shared_ptr<Model> makeModel(std::shared_ptr<Shape>, const pybind11::kwargs&);
