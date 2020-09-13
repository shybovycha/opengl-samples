#pragma once

#include "Level.h"
#include "ResourceManager.h"
#include "Settings.h"

#include <irrlicht.h>
#include <tinyxml2.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ModernResourceManager : public ResourceManager {
public:
    ModernResourceManager();

    virtual Settings loadSettings();

    virtual std::vector<std::shared_ptr<Level>> loadLevels();
};
