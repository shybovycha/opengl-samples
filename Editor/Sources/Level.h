#pragma once

#include <string>
#include <vector>

#include "irrlicht.h"

class Level {
public:
    Level(const std::wstring& meshFilename);

    const std::vector<irr::core::vector3df> getTargets() const;

    size_t addTargetPosition(irr::core::vector3df position);

    void deleteTargetAt(size_t index);

    void updateTargetAt(size_t index, irr::core::vector3df newPosition);

    std::wstring getMeshFilename() const;

private:
    std::vector<irr::core::vector3df> targets;
    std::wstring meshFilename;
};
