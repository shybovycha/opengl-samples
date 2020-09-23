#include "Light.h"

Light::Light(irr::core::vector3df _position, const std::wstring& _id) : id(_id), position(_position), sceneNode(nullptr) {}

irr::core::vector3df Light::getPosition() const {
    return position;
}

std::wstring Light::getId() const {
    return id;
}

void Light::setPosition(irr::core::vector3df newPosition) {
    position = newPosition;
}

void Light::setSceneNode(irr::scene::ISceneNode* _sceneNode) {
    sceneNode = _sceneNode;
}

irr::scene::ISceneNode* Light::getSceneNode() const {
    return sceneNode;
}
