#include "Level.h"

Level::Level(const std::string filename) : meshFilename(filename) {}

const std::string Level::getModelFilename() const {
    return meshFilename;
}

void Level::addTargetPosition(irr::core::vector3df position) {
    targetPositions.push_back(position);
}

const std::vector<irr::core::vector3df> Level::getTargetPositions() const {
    return targetPositions;
}

void Level::setModel(std::shared_ptr<irr::scene::ISceneNode> mesh) {
    model = mesh;
}

const std::shared_ptr<irr::scene::ISceneNode> Level::getModel() const {
    return model;
}

const std::vector<std::shared_ptr<irr::scene::ISceneNode>> Level::getTargets() const {
    return targets;
}

void Level::setTargets(std::vector<std::shared_ptr<irr::scene::ISceneNode>> _targets) {
    targets = _targets;
}
