#pragma once

#include <string>

#include "irrlicht.h"

class Target {
public:
    Target(irr::core::vector3df _position, const std::wstring& _id);

    irr::core::vector3df getPosition() const;

    std::wstring getId() const;

    void setPosition(irr::core::vector3df newPosition);

private:
    irr::core::vector3df position;
    std::wstring id;
};
