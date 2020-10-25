#pragma once

#include <codecvt> // for converting wstring to string
#include <xlocbuf> // for converting wstring to string

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <irrlicht.h>
#include <tinyxml2.h>

#include "GameManagerNodeData.h"
#include "Level.h"
#include "LevelEntity.h"
#include "Target.h"

class GameData
{
public:
    GameData(irr::IrrlichtDevice* _device);

    void loadFromFile(const std::wstring& filename);

    void saveToFile(const std::wstring& filename);

    std::shared_ptr<Level> getLevelById(const std::wstring& levelId) const;

    std::shared_ptr<Level> createLevel(const std::wstring& meshFilename);

    void deleteLevelById(const std::wstring& levelId);

    std::vector<std::shared_ptr<Level>> getLevels() const;

    std::shared_ptr<Level> getCurrentLevel() const;

    std::shared_ptr<LevelEntity> getCurrentEntity() const;

    void setCurrentEntity(std::shared_ptr<LevelEntity> entity);

    void setCurrentLevel(std::shared_ptr<Level> level);

private:
    irr::IrrlichtDevice* device;

    std::map<std::wstring, std::shared_ptr<Level>> levels;

    std::shared_ptr<Level> currentLevel;
    std::shared_ptr<LevelEntity> currentEntity;
};
