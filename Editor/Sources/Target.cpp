#include "Target.h"

Target::Target(irr::core::vector3df _position, const std::wstring& _id) : position(_position), id(_id) {}

irr::core::vector3df Target::getPosition() const {
    return position;
}

std::wstring Target::getId() const {
    return id;
}

void Target::setPosition(irr::core::vector3df newPosition) {
    position = newPosition;
}

void Target::setSceneNode(irr::scene::ISceneNode* _sceneNode) {
    sceneNode = _sceneNode;
}

irr::scene::ISceneNode* Target::getSceneNode() const {
    return sceneNode;
}
