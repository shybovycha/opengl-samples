#include <fstream>

#include "irrlicht.h"
#include "tinyxml2.h"

#pragma comment(lib, "irrlicht.lib")

irr::core::vector3df point[100];
int pointCnt = 0;
char* fileout = "";

irr::IrrlichtDevice* device = 0;
irr::video::IVideoDriver* driver = 0;
irr::scene::ISceneManager* smgr = 0;
irr::gui::IGUIEnvironment* guienv = 0;
irr::gui::IGUIStaticText* text = 0;
irr::scene::IAnimatedMesh* mesh = 0;
irr::scene::IAnimatedMeshSceneNode* node = 0;
irr::scene::ICameraSceneNode* camera = 0;
irr::scene::ILightSceneNode* light = 0;

void saveData(char* filename);

class EventReceiver : public irr::IEventReceiver
{
public:
    virtual bool OnEvent(const irr::SEvent& event) {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
                point[pointCnt++] = camera->getPosition();

                smgr->addSphereSceneNode(10, 64, 0, 0, camera->getPosition(),
                    irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));

                smgr->addLightSceneNode(0, point[pointCnt], irr::video::SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);
            }
        }

        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (event.KeyInput.Key == irr::KEY_ESCAPE) {
                device->drop();

                saveData(fileout);

                exit(1);
            }

            if (event.KeyInput.Key == irr::KEY_F2) {
                saveData(fileout);
            }
        }

        return false;
    }
};

void saveData(char* filename) {
    std::ofstream outf(filename);

    outf << pointCnt << std::endl;

    for (int i = 0; i <= pointCnt - 1; i++) {
        outf << point[i].X << " " << point[i].Y << " " << point[i].Z << std::endl;
    }

    outf.close();
}

int main(int argc, char* argv[]) {
    fileout = argv[2];

    EventReceiver receiver;

    device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480), 32,
        false, false, false, 0);

    device->setEventReceiver(&receiver);

    device->setWindowCaption(L"Shoot Them! Editor");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    light = smgr->addLightSceneNode(0, irr::core::vector3df(0, 0, 0), irr::video::SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);

    device->getCursorControl()->setVisible(false);

    text = guienv->addStaticText(L"", irr::core::rect<irr::s32>(10, 10, 200, 22), true);

    mesh = smgr->getMesh(argv[1]);
    node = smgr->addAnimatedMeshSceneNode(mesh);

    camera = smgr->addCameraSceneNodeFPS();

    while (device->run()) {
        driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

        smgr->drawAll();
        guienv->drawAll();

        irr::core::stringw str = L"Points: ";
        str += pointCnt;
        text->setText(str.c_str());

        light->setPosition(camera->getPosition());

        driver->endScene();
    }

    device->drop();

    return 0;
}
