#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "irrlicht.h"

#include "Target.h"

class Level {
public:
    Level(const std::wstring& meshFilename, const std::wstring& id);

    Level(const std::wstring& meshFilename, const std::wstring& id, const std::wstring& meshBasename);

    void setSceneNode(irr::scene::ISceneNode* sceneNode);

    irr::scene::ISceneNode* getSceneNode() const;

    const std::vector<std::shared_ptr<Target>> getTargets() const;

    const std::shared_ptr<Target> getTargetById(std::wstring targetId) const;

    std::shared_ptr<Target> createTarget(irr::core::vector3df position);

    void deleteTargetById(std::wstring targetId);

    void updateTargetById(std::wstring targetId, irr::core::vector3df newPosition);

    std::wstring getMeshFilename() const;
    
    std::wstring getMeshBasename() const;

    std::wstring getId() const;

private:
    const std::vector<std::shared_ptr<Target>>::const_iterator getTargetIteratorById(std::wstring targetId) const;

    std::vector<std::shared_ptr<Target>> targets;
    std::wstring meshFilename;
    std::wstring meshBasename;
    std::wstring id;

    irr::scene::ISceneNode* sceneNode;
};
