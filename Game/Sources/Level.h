#pragma once

#include <memory>
#include <string>
#include <vector>

#include <irrlicht.h>

class Level {
public:
    Level(const std::string filename);

    const std::string getModelFilename() const;

    void addTargetPosition(irr::core::vector3df position);

    const std::vector<irr::core::vector3df> getTargetPositions() const;

    void setModel(std::shared_ptr<irr::scene::ISceneNode> mesh);

    const std::shared_ptr<irr::scene::ISceneNode> getModel() const;

    const std::vector<std::shared_ptr<irr::scene::ISceneNode>> getTargets() const;

    void setTargets(std::vector<std::shared_ptr<irr::scene::ISceneNode>> _targets);

private:
    std::string meshFilename;

    std::vector<irr::core::vector3df> targetPositions;
    std::vector<std::shared_ptr<irr::scene::ISceneNode>> targets;

    irr::core::vector3df playerPosition;

    std::shared_ptr<irr::scene::ISceneNode> model;
};
