#pragma once

#include <string>

struct Settings {
    std::string driverName;
    int resolutionWidth;
    int resolutionHeight;
    int colorDepth;
    bool fullScreen;
    bool vsync;
    bool stencil;
};
