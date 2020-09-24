#pragma once

#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <irrlicht.h>

#include "LevelEntity.h"
#include "Light.h"
#include "Target.h"

class Level {
public:
    Level(const std::wstring& meshFilename, const std::wstring& id);

    Level(const std::wstring& meshFilename, const std::wstring& id, const std::wstring& meshBasename);

    ~Level();

    void setSceneNode(irr::scene::ISceneNode* sceneNode);

    irr::scene::ISceneNode* getSceneNode() const;

    const std::vector<std::shared_ptr<LevelEntity>> getEntities() const;

    const std::shared_ptr<LevelEntity> getEntityById(std::wstring entityId) const;

    std::shared_ptr<Target> createTarget(irr::core::vector3df position);
    
    std::shared_ptr<Light> createLight(irr::core::vector3df position);

    void deleteEntityById(std::wstring entityId);

    void updateEntityById(std::wstring entityId, irr::core::vector3df newPosition);

    std::wstring getMeshFilename() const;
    
    std::wstring getMeshBasename() const;

    std::wstring getId() const;

private:
    std::wstring generateNewEntityId(LevelEntityType entityType) const;

    std::map<std::wstring, std::shared_ptr<LevelEntity>> entities;

    std::wstring meshFilename;
    std::wstring meshBasename;
    std::wstring id;

    irr::scene::ISceneNode* sceneNode;
};
