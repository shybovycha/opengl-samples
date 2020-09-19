#include <fstream>
#include <memory>
#include <vector>

#include "irrlicht.h"
#include "tinyxml2.h"

#pragma comment(lib, "irrlicht.lib")

const std::string LEVEL_FILENAME = "level.dat";

class ApplicationDelegate {
public:
    ApplicationDelegate(std::shared_ptr<irr::IrrlichtDevice> _device) :
        device(_device), 
        smgr(std::shared_ptr<irr::scene::ISceneManager>(device->getSceneManager())), 
        guienv(std::shared_ptr<irr::gui::IGUIEnvironment>(device->getGUIEnvironment())),
        driver(std::shared_ptr<irr::video::IVideoDriver>(device->getVideoDriver()))
    {}

    void init() {
        camera = std::shared_ptr<irr::scene::ICameraSceneNode>(smgr->addCameraSceneNodeFPS());
    }

    void update() {
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

    void placeTarget() {
        points.push_back(camera->getPosition());

        smgr->addSphereSceneNode(10, 64, 0, 0, camera->getPosition(),
            irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));

        // smgr->addLightSceneNode(0, point[pointCnt], irr::video::SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);
    }

    void placeLight() {}

    void saveLevel() {
        device->drop();

        saveData(LEVEL_FILENAME);
    }

    void quit() {
        device->drop();
    }

protected:
    void saveData(const std::string& filename) {
        std::ofstream outf(filename);

        outf << points.size() << std::endl;

        for (irr::core::vector3df point : points) {
            outf << point.X << " " << point.Y << " " << point.Z << std::endl;
        }

        outf.close();
    }

private:
    std::shared_ptr<irr::IrrlichtDevice> device;
    std::shared_ptr<irr::video::IVideoDriver> driver;
    std::shared_ptr<irr::scene::ISceneManager> smgr;
    std::shared_ptr<irr::gui::IGUIEnvironment> guienv;
    std::shared_ptr<irr::scene::ICameraSceneNode> camera;
    std::vector<irr::core::vector3df> points;
};

class EventReceiver : public irr::IEventReceiver {
public:
    EventReceiver(std::shared_ptr<ApplicationDelegate> _delegate) : delegate(_delegate) {}

    virtual bool OnEvent(const irr::SEvent& event) {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
                delegate->placeTarget();
            }
        }

        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (event.KeyInput.Key == irr::KEY_ESCAPE) {
                delegate->saveLevel();
                delegate->quit();
            }

            if (event.KeyInput.Key == irr::KEY_F2) {
                delegate->saveLevel();
            }
        }

        return false;
    }

private:
    std::shared_ptr<ApplicationDelegate> delegate;
};

int main(int argc, char* argv[]) {
    std::shared_ptr<irr::IrrlichtDevice> device(irr::createDevice(
        irr::video::EDT_OPENGL, 
        irr::core::dimension2d<irr::u32>(640, 480), 
        32,
        false, 
        false, 
        false, 
        0
    ));

    device->setWindowCaption(L"Shoot Them! Editor");

    std::unique_ptr<ApplicationDelegate> delegate = std::make_unique<ApplicationDelegate>(device);

    std::unique_ptr<EventReceiver> receiver = std::make_unique<EventReceiver>(delegate);

    device->setEventReceiver(receiver.get());

    delegate->init();

    // irr::scene::ILightSceneNode* light = smgr->addLightSceneNode(0, irr::core::vector3df(0, 0, 0), irr::video::SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);

    // device->getCursorControl()->setVisible(false);

    // irr::gui::IGUIStaticText* text = guienv->addStaticText(L"", irr::core::rect<irr::s32>(10, 10, 200, 22), true);

    /*irr::scene::IAnimatedMesh* mesh = smgr->getMesh(argv[1]);
    irr::scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);*/

    while (device->run()) {
        delegate->update();
    }

    device->drop();

    return 0;
}
