#include "Level.h"

Level::Level(const std::wstring& _meshFilename, const std::wstring& _id) : meshFilename(_meshFilename), id(_id), meshBasename(_meshFilename), sceneNode(nullptr) {}

Level::Level(const std::wstring& _meshFilename, const std::wstring& _id, const std::wstring& _meshBasename) : meshFilename(_meshFilename), id(_id), meshBasename(_meshBasename), sceneNode(nullptr) {}

const std::vector<std::shared_ptr<Target>> Level::getTargets() const {
    return targets;
}

std::shared_ptr<Target> Level::createTarget(irr::core::vector3df position) {
    size_t targetIndex = targets.size();
    std::wostringstream idString;
    idString << id << "-target-" << targetIndex;

    std::shared_ptr<Target> target = std::make_shared<Target>(position, idString.str());
    targets.push_back(target);
    
    return target;
}

void Level::deleteTargetById(std::wstring targetId) {
    auto it = getTargetIteratorById(targetId);

    if (it != targets.end()) {
        targets.erase(it);
    }
}

void Level::updateTargetById(std::wstring targetId, irr::core::vector3df newPosition) {
    auto it = getTargetIteratorById(targetId);

    if (it != targets.end()) {
        (*it)->setPosition(newPosition);
    }
}

const std::vector<std::shared_ptr<Target>>::const_iterator Level::getTargetIteratorById(std::wstring targetId) const {
    return std::find_if(targets.begin(), targets.end(), [&](const std::shared_ptr<Target>& target) { return target->getId() == targetId; });
}

const std::shared_ptr<Target> Level::getTargetById(std::wstring targetId) const {
    auto targetIt = getTargetIteratorById(targetId);

    if (targetIt != targets.end()) {
        return *targetIt;
    }

    return nullptr;
}

const std::vector<std::shared_ptr<Light>> Level::getLights() const {
    return lights;
}

const std::vector<std::shared_ptr<Light>>::const_iterator Level::getLightIteratorById(std::wstring lightId) const {
    return std::find_if(lights.begin(), lights.end(), [&](const std::shared_ptr<Light>& light) { return light->getId() == lightId; });
}

const std::shared_ptr<Light> Level::getLightById(std::wstring lightId) const {
    auto lightIt = getLightIteratorById(lightId);

    if (lightIt != lights.end()) {
        return *lightIt;
    }

    return nullptr;
}

std::shared_ptr<Light> Level::createLight(irr::core::vector3df position) {
    size_t lightIndex = lights.size();
    std::wostringstream idString;
    idString << id << "-light-" << lightIndex;

    std::shared_ptr<Light> light = std::make_shared<Light>(position, idString.str());
    lights.push_back(light);

    return light;
}

void Level::deleteLightById(std::wstring lightId) {
    auto it = getLightIteratorById(lightId);

    if (it != lights.end()) {
        lights.erase(it);
    }
}

void Level::updateLightById(std::wstring lightId, irr::core::vector3df newPosition) {
    auto it = getLightIteratorById(lightId);

    if (it != lights.end()) {
        (*it)->setPosition(newPosition);
    }
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
