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

void Level::setModel(irr::scene::ISceneNode* mesh) {
    model = mesh;
}

irr::scene::ISceneNode* Level::getModel() const {
    return model;
}

const std::vector<irr::scene::ISceneNode*> Level::getTargets() const {
    return targets;
}

void Level::setTargets(std::vector<irr::scene::ISceneNode*> _targets) {
    targets = _targets;
}
