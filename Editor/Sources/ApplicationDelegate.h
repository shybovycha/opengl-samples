#pragma once
#pragma comment(lib, "irrlicht.lib")

#include <fstream>
#include <memory>
#include <vector>

#include "irrlicht.h"
#include "tinyxml2.h"

const std::string LEVEL_FILENAME = "level.dat";

class ApplicationDelegate {
public:
    ApplicationDelegate(std::shared_ptr<irr::IrrlichtDevice> _device);

    void init();

    void update();

    void placeTarget();

    void placeLight();

    void saveLevel();

    void quit();

protected:
    void saveData(const std::string& filename);

private:
    std::shared_ptr<irr::IrrlichtDevice> device;
    std::shared_ptr<irr::video::IVideoDriver> driver;
    std::shared_ptr<irr::scene::ISceneManager> smgr;
    std::shared_ptr<irr::gui::IGUIEnvironment> guienv;
    std::shared_ptr<irr::scene::ICameraSceneNode> camera;
    std::vector<irr::core::vector3df> points;
};
