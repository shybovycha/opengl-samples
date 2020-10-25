#pragma once

#include <string>

#include <irrlicht.h>

#include "LevelEntity.h"

class Target : public LevelEntity
{
public:
    Target(const irr::core::vector3df& _position, const std::wstring& _id);
};
