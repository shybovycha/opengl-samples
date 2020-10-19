#pragma once

#include <string>
#include <vector>

#include "stdafx.h"

class Level {
public:
    Level(const std::string filename);

    std::string getModelFilename() const;

    void addTargetPosition(irr::core::vector3df position);

    void addLightPosition(irr::core::vector3df position);

    std::vector<irr::core::vector3df> getTargetPositions() const;

    std::vector<irr::core::vector3df> getLightPositions() const;

    void setModel(irr::scene::ISceneNode* mesh);

    irr::scene::ISceneNode* getModel() const;

    std::vector<irr::scene::ISceneNode*> getTargets() const;

    std::vector<irr::scene::ISceneNode*> getLights() const;

    void setTargets(std::vector<irr::scene::ISceneNode*> targets);

    void setLights(std::vector<irr::scene::ISceneNode*> lights);

private:
    std::string meshFilename;

    std::vector<irr::core::vector3df> targetPositions;
    std::vector<irr::core::vector3df> lightPositions;
    std::vector<irr::scene::ISceneNode*> targets;
    std::vector<irr::scene::ISceneNode*> lights;

    irr::core::vector3df playerPosition;

    irr::scene::ISceneNode* model;
};
