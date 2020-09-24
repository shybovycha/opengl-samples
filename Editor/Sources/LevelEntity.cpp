#include "LevelEntity.h"

LevelEntity::LevelEntity(irr::core::vector3df _position, std::wstring _id, LevelEntityType _type) : id(_id), position(_position), type(_type), sceneNode(nullptr) {}

LevelEntity::~LevelEntity() {
    if (sceneNode != nullptr) {
        sceneNode->setVisible(false);
    }
}

irr::core::vector3df LevelEntity::getPosition() const {
    return position;
}

std::wstring LevelEntity::getId() const {
    return id;
}

void LevelEntity::setPosition(irr::core::vector3df newPosition) {
    position = newPosition;
}

void LevelEntity::setSceneNode(irr::scene::ISceneNode* _sceneNode) {
    sceneNode = _sceneNode;
}

irr::scene::ISceneNode* LevelEntity::getSceneNode() const {
    return sceneNode;
}

LevelEntityType LevelEntity::getType() const {
    return type;
}
