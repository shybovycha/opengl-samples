#include "Level.h"

Level::Level(const std::wstring& _meshFilename, const std::wstring& _id) : meshFilename(_meshFilename), id(_id), meshBasename(_meshFilename), sceneNode(nullptr) {}

Level::Level(const std::wstring& _meshFilename, const std::wstring& _id, const std::wstring& _meshBasename) : meshFilename(_meshFilename), id(_id), meshBasename(_meshBasename), sceneNode(nullptr) {}

const std::vector<std::shared_ptr<LevelEntity>> Level::getEntities() const {
    std::vector<std::shared_ptr<LevelEntity>> result;

    for (auto const& it : entities) {
        result.push_back(it.second);
    }

    return result;
}

const std::shared_ptr<LevelEntity> Level::getEntityById(std::wstring entityId) const {
    return entities.at(entityId);
}

std::shared_ptr<Target> Level::createTarget(irr::core::vector3df position) {
    std::wstring id = generateNewEntityId(LevelEntityType::TARGET);
    std::shared_ptr<Target> target = std::make_shared<Target>(position, id);
    
    entities[id] = target;
    
    return target;
}

std::shared_ptr<Light> Level::createLight(irr::core::vector3df position) {
    std::wstring id = generateNewEntityId(LevelEntityType::LIGHT);
    std::shared_ptr<Light> light = std::make_shared<Light>(position, id);

    entities[id] = light;

    return light;
}

std::wstring Level::generateNewEntityId(LevelEntityType entityType) const {
    size_t entityIndex = entities.size();
    
    std::wostringstream idString;

    idString << id << "-";
    
    if (entityType == LevelEntityType::LIGHT) {
        idString << "light";
    }
    else if (entityType == LevelEntityType::TARGET) {
        idString << "target";
    }
    else {
        idString << "unknown";
    }

    idString << "-" << entityIndex;

    return idString.str();
}

void Level::deleteEntityById(std::wstring entityId) {
    entities.erase(entityId);
}

void Level::updateEntityById(std::wstring entityId, irr::core::vector3df newPosition) {
    entities[entityId]->setPosition(newPosition);
}

std::wstring Level::getMeshFilename() const {
    return meshFilename;
}

std::wstring Level::getMeshBasename() const {
    return meshBasename;
}

std::wstring Level::getId() const {
    return id;
}

void Level::setSceneNode(irr::scene::ISceneNode* _sceneNode) {
    sceneNode = _sceneNode;
}

irr::scene::ISceneNode* Level::getSceneNode() const {
    return sceneNode;
}
