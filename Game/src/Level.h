#pragma once

#include <string>
#include <vector>

#include "stdafx.h"

#include "Vector3.h"

using Vector3df = Vector3<float>;

class Level
{
public:
    Level(const std::string filename);

    std::string getModelFilename() const;

    void addTargetPosition(Vector3df position);

    void addLightPosition(Vector3df position);

    std::vector<Vector3df> getTargetPositions() const;

    std::vector<Vector3df> getLightPositions() const;

    void setModel(irr::scene::ISceneNode* mesh);

    irr::scene::ISceneNode* getModel() const;

    std::vector<irr::scene::ISceneNode*> getTargets() const;

    std::vector<irr::scene::ISceneNode*> getLights() const;

    void setTargets(std::vector<irr::scene::ISceneNode*> targets);

    void setLights(std::vector<irr::scene::ISceneNode*> lights);

private:
    std::string meshFilename;

    std::vector<Vector3df> targetPositions;
    std::vector<Vector3df> lightPositions;
    std::vector<irr::scene::ISceneNode*> targets;
    std::vector<irr::scene::ISceneNode*> lights;

    irr::scene::ISceneNode* model;
};
