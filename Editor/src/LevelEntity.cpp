#include "LevelEntity.h"

LevelEntity::LevelEntity(const irr::core::vector3df& _position, const std::wstring& _id, LevelEntityType _type) : id(
        _id), position(_position), type(_type), sceneNode(nullptr)
{
}

irr::core::vector3df LevelEntity::getPosition() const
{
    return position;
}

std::wstring LevelEntity::getId() const
{
    return id;
}

void LevelEntity::setPosition(const irr::core::vector3df& newPosition)
{
    position = newPosition;

    if (sceneNode != nullptr)
    {
        sceneNode->setPosition(newPosition);
    }
}

void LevelEntity::setSceneNode(irr::scene::ISceneNode* _sceneNode)
{
    sceneNode = _sceneNode;
}

irr::scene::ISceneNode* LevelEntity::getSceneNode() const
{
    return sceneNode;
}

LevelEntityType LevelEntity::getType() const
{
    return type;
}
