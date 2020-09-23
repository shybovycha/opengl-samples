#pragma once

#include <string>

#include "irrlicht.h"

class Light {
public:
    Light(irr::core::vector3df _position, const std::wstring& _id);

    irr::core::vector3df getPosition() const;

    std::wstring getId() const;

    void setPosition(irr::core::vector3df newPosition);

    void setSceneNode(irr::scene::ISceneNode* sceneNode);

    irr::scene::ISceneNode* getSceneNode() const;

private:
    irr::core::vector3df position;
    std::wstring id;

    irr::scene::ISceneNode* sceneNode;
};
