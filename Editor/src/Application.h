#pragma once

#include <memory>

#include <irrlicht.h>

#include "ApplicationDelegate.h"
#include "IrrlichtEventReceiver.h"

class Application
{
public:
    Application();

    void run();

private:
    irr::IrrlichtDevice* device;
};
