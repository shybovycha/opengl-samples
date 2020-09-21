#pragma once

#include <codecvt> // for converting wstring to string
#include <xlocbuf> // for converting wstring to string

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "irrlicht.h"
#include "tinyxml2.h"

#include "GameManagerNodeData.h"
#include "Level.h"
#include "Target.h"

class GameData {
public:
    GameData(irr::IrrlichtDevice* _device);

    void loadFromFile(const std::wstring& filename);

    void saveToFile(const std::wstring& filename);

    std::shared_ptr<Level> getLevelById(const std::wstring& levelId) const;

    std::shared_ptr<Level> createLevel(const std::wstring& meshFilename);

    std::vector<std::shared_ptr<Level>> getLevels() const;

private:
    irr::IrrlichtDevice* device;

    std::vector<std::shared_ptr<Level>> levels;
};
