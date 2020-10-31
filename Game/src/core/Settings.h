#pragma once

#include <string>

class Settings
{
public:
    Settings(
            std::string driverName,
            int resolutionWidth,
            int resolutionHeight,
            int colorDepth,
            bool fullScreen,
            bool vsync,
            bool stencil
    ) :
            driverName(driverName),
            resolutionWidth(resolutionWidth),
            resolutionHeight(resolutionHeight),
            colorDepth(colorDepth),
            fullScreen(fullScreen),
            vsync(vsync),
            stencil(stencil)
    {
    }

    std::string driverName;
    int resolutionWidth;
    int resolutionHeight;
    int colorDepth;
    bool fullScreen;
    bool vsync;
    bool stencil;
};
