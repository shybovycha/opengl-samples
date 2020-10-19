#pragma once

#include <memory>
#include <vector>

#include "Settings.h"

class ResourceManager {
public:
    ResourceManager() {}

    virtual Settings loadSettings() = 0;

    virtual std::vector<std::shared_ptr<Level>> loadLevels() = 0;
};
