#pragma once

#include <string>
#include <vector>

#include <irrlicht.h>

class Level {
public:
    Level(const std::string filename);

    const std::string getModelFilename() const;

    void addTargetPosition(irr::core::vector3df position);

    const std::vector<irr::core::vector3df> getTargetPositions() const;

    void setModel(irr::scene::ISceneNode* mesh);

    irr::scene::ISceneNode* getModel() const;

    const std::vector<irr::scene::ISceneNode*> getTargets() const;

    void setTargets(std::vector<irr::scene::ISceneNode*> _targets);

private:
    std::string meshFilename;

    std::vector<irr::core::vector3df> targetPositions;
    std::vector<irr::scene::ISceneNode*> targets;

    irr::core::vector3df playerPosition;

    irr::scene::ISceneNode* model;
};
