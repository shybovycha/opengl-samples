#include "ApplicationDelegate.h"

ApplicationDelegate::ApplicationDelegate(std::shared_ptr<irr::IrrlichtDevice> _device) :
    device(_device),
    smgr(std::shared_ptr<irr::scene::ISceneManager>(device->getSceneManager())),
    guienv(std::shared_ptr<irr::gui::IGUIEnvironment>(device->getGUIEnvironment())),
    driver(std::shared_ptr<irr::video::IVideoDriver>(device->getVideoDriver())),
    levelsFilename(std::nullopt),
    loadLevelsDialogIsShown(false),
    saveLevelsDialogIsShown(false),
    aboutWindowIsShown(false)
{}

void ApplicationDelegate::init() {
    camera = std::shared_ptr<irr::scene::ICameraSceneNode>(smgr->addCameraSceneNodeMaya());

    /*irr::gui::IGUIContextMenu* menu = guienv->addMenu();
    menu->addItem(L"File", -1, true, true);
    menu->addItem(L"Add", -1, true, true);

    irr::gui::IGUIContextMenu* fileSubmenu = menu->getSubMenu(0);
    fileSubmenu->addItem(L"Load game levels", static_cast<irr::s32>(GuiElementID::LOAD_LEVELS));
    fileSubmenu->addItem(L"Save game levels", static_cast<irr::s32>(GuiElementID::SAVE_LEVELS));
    fileSubmenu->addSeparator();
    fileSubmenu->addItem(L"About", static_cast<irr::s32>(GuiElementID::ABOUT));
    fileSubmenu->addSeparator();
    fileSubmenu->addItem(L"Quit", static_cast<irr::s32>(GuiElementID::QUIT));

    irr::gui::IGUIContextMenu* addSubmenu = menu->getSubMenu(1);
    addSubmenu->addItem(L"Level", static_cast<irr::s32>(GuiElementID::ADD_LEVEL));
    addSubmenu->addItem(L"Target", static_cast<irr::s32>(GuiElementID::ADD_TARGET));*/

    // ------------------------------------------

    irr::gui::IGUIToolBar* toolbar = guienv->addToolBar();
    toolbar->setMinSize(irr::core::dimension2du(100, 40));

    irr::video::ITexture* openFileIcon = driver->getTexture("Resources/Icons/opened-folder.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::LOAD_LEVELS), nullptr, L"Load game levels", openFileIcon);

    irr::video::ITexture* saveFileIcon = driver->getTexture("Resources/Icons/save.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::SAVE_LEVELS), nullptr, L"Save game levels", saveFileIcon);

    irr::video::ITexture* helpIcon = driver->getTexture("Resources/Icons/help.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::ABOUT), nullptr, L"About", helpIcon);

    irr::video::ITexture* addLevelIcon = driver->getTexture("Resources/Icons/map.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::ADD_LEVEL), nullptr, L"Add level", addLevelIcon);

    irr::video::ITexture* addTargetIcon = driver->getTexture("Resources/Icons/map-pin.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::ADD_TARGET), nullptr, L"Add target", addTargetIcon);

}

void ApplicationDelegate::update() {
    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
}

void ApplicationDelegate::placeTarget() {
    // TODO: replace with raycasting, falling back to cameraPosition + cameraDirection * targetModelSize * X
    targets.push_back(camera->getPosition());

    // TODO: replace with actual target model
    smgr->addSphereSceneNode(10, 64, 0, 0, camera->getPosition(), irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));

    // smgr->addLightSceneNode(0, point[pointCnt], irr::video::SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);
}

void ApplicationDelegate::placeLight() {
    // TODO: implement
}

void ApplicationDelegate::saveLevels() {
    if (levelsFilename != std::nullopt) {
        saveLevels(*levelsFilename);
    }
    else {
        openSaveLevelsDialog();
    }
}

void ApplicationDelegate::saveLevels(const std::wstring& filename) {
    // TODO: rework to XML
    // TODO: support multiple levels
    std::ofstream outf(filename);

    outf << targets.size() << std::endl;

    for (irr::core::vector3df point : targets) {
        outf << point.X << " " << point.Y << " " << point.Z << std::endl;
    }

    outf.close();

    loadLevelsDialogIsShown = false;
    levelsFilename = filename;
}

void ApplicationDelegate::openSaveLevelsDialog() {
    if (saveLevelsDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Save levels file");
    saveLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeSaveLevelsDialog() {
    saveLevelsDialogIsShown = false;
}

void ApplicationDelegate::openLoadLevelsDialog() {
    if (loadLevelsDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Load levels file");
    loadLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeLoadLevelsDialog() {
    loadLevelsDialogIsShown = false;
}

void ApplicationDelegate::loadLevels(const std::wstring& filename) {
    // TODO: implement
    loadLevelsDialogIsShown = false;
}

void ApplicationDelegate::openAboutWindow() {
    guienv->addMessageBox(L"About", ABOUT_TEXT.c_str());
    aboutWindowIsShown = true;
}

void ApplicationDelegate::closeAboutWindow() {
    aboutWindowIsShown = false;
}

void ApplicationDelegate::addLevel() {}

void ApplicationDelegate::quit() {
    device->closeDevice();
}
