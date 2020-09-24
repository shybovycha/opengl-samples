#pragma once

#include <string>

#include <irrlicht.h>

#include "LevelEntity.h"

class Light : public LevelEntity {
public:
    Light(irr::core::vector3df _position, const std::wstring& _id);
};
