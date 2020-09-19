#include "ApplicationDelegate.h"

ApplicationDelegate::ApplicationDelegate(std::shared_ptr<irr::IrrlichtDevice> _device) :
    device(_device),
    smgr(std::shared_ptr<irr::scene::ISceneManager>(device->getSceneManager())),
    guienv(std::shared_ptr<irr::gui::IGUIEnvironment>(device->getGUIEnvironment())),
    driver(std::shared_ptr<irr::video::IVideoDriver>(device->getVideoDriver()))
{}

void ApplicationDelegate::init() {
    camera = std::shared_ptr<irr::scene::ICameraSceneNode>(smgr->addCameraSceneNodeFPS());
}

void ApplicationDelegate::update() {
    // TODO: update UI

    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    smgr->drawAll();
    guienv->drawAll();

    // irr::core::stringw str = L"Points: ";
    // str += pointCnt;
    // text->setText(str.c_str());

    // light->setPosition(camera->getPosition());

    driver->endScene();
}

void ApplicationDelegate::placeTarget() {
    points.push_back(camera->getPosition());

    smgr->addSphereSceneNode(10, 64, 0, 0, camera->getPosition(),
        irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));

    // smgr->addLightSceneNode(0, point[pointCnt], irr::video::SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);
}

void ApplicationDelegate::placeLight() {
    // TODO: implement
}

void ApplicationDelegate::saveLevel() {
    device->drop();

    saveData(LEVEL_FILENAME);
}

void ApplicationDelegate::quit() {
    device->drop();
}

void ApplicationDelegate::saveData(const std::string& filename) {
    std::ofstream outf(filename);

    outf << points.size() << std::endl;

    for (irr::core::vector3df point : points) {
        outf << point.X << " " << point.Y << " " << point.Z << std::endl;
    }

    outf.close();
}

