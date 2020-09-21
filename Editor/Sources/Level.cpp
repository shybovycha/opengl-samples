#include "Level.h"

Level::Level(const std::wstring& _meshFilename, const std::wstring& _id) : meshFilename(_meshFilename), id(_id) {}

const std::vector<std::shared_ptr<Target>> Level::getTargets() const {
    return targets;
}

std::shared_ptr<Target> Level::addTargetPosition(irr::core::vector3df position, std::wstring targetId) {
    
    std::shared_ptr<Target> target = std::make_shared<Target>(position, targetId);
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

std::wstring Level::getMeshFilename() const {
    return meshFilename;
}

std::wstring Level::getId() const {
    return id;
}
