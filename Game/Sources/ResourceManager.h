#pragma once

#include "Settings.h"

#include <memory>
#include <vector>

class ResourceManager {
public:
    ResourceManager() {}

    virtual Settings loadSettings() = 0;

    virtual std::vector<std::shared_ptr<Level>> loadLevels() = 0;
};
