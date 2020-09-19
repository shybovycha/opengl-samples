#include "Application.h"

Application::Application() {}

void Application::run() {
    device = std::shared_ptr<irr::IrrlichtDevice>(irr::createDevice(
        irr::video::EDT_OPENGL,
        irr::core::dimension2d<irr::u32>(640, 480),
        32,
        false,
        false,
        false,
        0
    ));

    device->setWindowCaption(L"Shoot Them! Editor");

    std::shared_ptr<ApplicationDelegate> delegate = std::make_shared<ApplicationDelegate>(device);

    std::shared_ptr<IrrlichtEventReceiver> receiver = std::make_shared<IrrlichtEventReceiver>(delegate);

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
}
