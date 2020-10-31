#include "Level.h"

Level::Level(const std::string filename) : meshFilename(filename), model(nullptr)
{
}

std::string Level::getModelFilename() const
{
    return meshFilename;
}

void Level::addTargetPosition(Vector3<double> position)
{
    targetPositions.push_back(position);
}

std::vector<Vector3<double>> Level::getTargetPositions() const
{
    return targetPositions;
}

void Level::setModel(SceneNode* _model)
{
    model = _model;
}

SceneNode* Level::getModel() const
{
    return model;
}

std::vector<SceneNode*> Level::getTargets() const
{
    return targets;
}

void Level::setTargets(std::vector<SceneNode*> _targets)
{
    targets = _targets;
}

void Level::addLightPosition(Vector3<double> position)
{
    lightPositions.push_back(position);
}

std::vector<Vector3<double>> Level::getLightPositions() const
{
    return lightPositions;
}

void Level::setLights(std::vector<SceneNode*> _lights)
{
    lights = _lights;
}

std::vector<SceneNode*> Level::getLights() const
{
    return lights;
}
