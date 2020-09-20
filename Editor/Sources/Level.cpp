#include "Level.h"

Level::Level(const std::wstring& _meshFilename) : meshFilename(_meshFilename) {}

const std::vector<irr::core::vector3df> Level::getTargets() const {
    return targets;
}

size_t Level::addTarget(irr::core::vector3df position) {
    targets.push_back(position);
    return targets.size() - 1;
}

void Level::deleteTargetAt(size_t index) {
    targets.erase(targets.begin() + index);
}

void Level::updateTargetAt(size_t index, irr::core::vector3df newPosition) {
    targets[index] = newPosition;
}

std::wstring Level::getMeshFilename() const {
    return meshFilename;
}
