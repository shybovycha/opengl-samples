#include "ApplicationDelegate.h"

ApplicationDelegate::ApplicationDelegate(irr::IrrlichtDevice* _device) :
    device(_device),
    smgr(device->getSceneManager()),
    guienv(device->getGUIEnvironment()),
    driver(device->getVideoDriver()),
    levelsFilename(std::nullopt),
    loadLevelsDialogIsShown(false),
    saveLevelsDialogIsShown(false),
    aboutWindowIsShown(false),
    loadLevelMeshDialogIsShown(false)
{}

void ApplicationDelegate::init() {
    camera = smgr->addCameraSceneNodeMaya();

    initUI();
}

void ApplicationDelegate::initUI() {
    setFont();

    createToolbar();

    createManagerWindow();
}

void ApplicationDelegate::createToolbar() {
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

void ApplicationDelegate::createManagerWindow() {
    irr::gui::IGUIWindow* managerWindow = guienv->addWindow(
        irr::core::rect<irr::s32>(600, 70, 800, 470),
        false, 
        L"Levels manager", 
        0, 
        static_cast<irr::s32>(GuiElementID::MANAGER_WINDOW)
    );

    irr::gui::IGUITreeView* gameTree = guienv->addTreeView(
        irr::core::rect<irr::s32>(10, 30, 190, 390), 
        managerWindow, 
        static_cast<irr::s32>(GuiElementID::GAME_LEVEL_TREE)
    );
}

void ApplicationDelegate::update() {
    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
}

void ApplicationDelegate::placeTarget() {
    if (!currentLevelIndex) {
        guienv->addMessageBox(L"Error", L"You have to have at least one level before placing a target");
        return;
    }

    irr::core::triangle3df triangle;
    irr::core::vector3df collisionPoint;
    irr::core::line3df ray(camera->getAbsolutePosition(), camera->getAbsolutePosition() + camera->getTarget() * camera->getFarValue());

    const float PICK_DISTANCE = 20.f;
    
    irr::scene::ISceneNode* collisionNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collisionPoint, triangle);

    irr::core::vector3df targetPosition = camera->getAbsolutePosition() + camera->getTarget() * PICK_DISTANCE;

    if (collisionNode) {
        targetPosition = collisionPoint;
    }

    size_t targetId = levels[currentLevelIndex]->addTarget(targetPosition);

    std::wostringstream idString;
    idString << "level-" << currentLevelIndex << "-target-" << targetId;

    GameManagerNodeData* targetNodeData = new GameManagerNodeData(GameManagerNodeDataType::TARGET, idString.str());

    addManagerTreeNodeToRootNode(idString.str().c_str(), targetNodeData);

    // TODO: replace with actual target model
    smgr->addSphereSceneNode(10, 64, 0, 0, targetPosition, irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));
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
    irr::io::IXMLWriter* writer = device->getFileSystem()->createXMLWriter(filename.c_str());

    writer->writeElement(L"levels");

    for (std::shared_ptr<Level> level : levels) {
        writer->writeElement(L"level");
        
        writer->writeElement(L"model");

        irr::io::path path = device->getFileSystem()->getFileBasename(level->getMeshFilename().c_str());
        std::wostringstream meshFilename;
        meshFilename << path.c_str();

        writer->writeText(meshFilename.str().c_str());
        writer->writeClosingTag(L"model");
        
        writer->writeElement(L"targets");

        for (irr::core::vector3df target : level->getTargets()) {
            writer->writeElement(L"target");

            std::wostringstream positionX;
            positionX << target.X;

            std::wostringstream positionY;
            positionY << target.Y;

            std::wostringstream positionZ;
            positionZ << target.Z;

            writer->writeElement(L"position", true, L"x", positionX.str().c_str(), L"y", positionY.str().c_str(), L"z", positionZ.str().c_str());

            writer->writeClosingTag(L"target");
        }

        writer->writeClosingTag(L"targets");

        writer->writeClosingTag(L"level");
    }

    writer->writeClosingTag(L"levels");

    loadLevelsDialogIsShown = false;
    levelsFilename = filename;
}

void ApplicationDelegate::openSaveLevelsDialog() {
    if (saveLevelsDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Save levels file", true, 0, static_cast<irr::s32>(GuiElementID::SAVE_LEVELS_DIALOG));
    saveLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeSaveLevelsDialog() {
    saveLevelsDialogIsShown = false;
}

void ApplicationDelegate::openLoadLevelsDialog() {
    if (loadLevelsDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Load levels file", true, 0, static_cast<irr::s32>(GuiElementID::LOAD_LEVELS_DIALOG));
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

void ApplicationDelegate::openLoadLevelMeshDialog() {
    if (loadLevelMeshDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Load level mesh", true, 0, static_cast<irr::s32>(GuiElementID::LOAD_LEVEL_MESH_DIALOG));

    loadLevelMeshDialogIsShown = true;
}

void ApplicationDelegate::closeLoadLevelMeshDialog() {
    loadLevelMeshDialogIsShown = false;
}

void ApplicationDelegate::addLevel(const std::wstring& meshFilename) {
    levels.push_back(std::make_shared<Level>(meshFilename));

    std::wostringstream idString;
    idString << L"level-" << (levels.size() - 1);

    GameManagerNodeData* levelNodeData = new GameManagerNodeData(GameManagerNodeDataType::LEVEL, idString.str());

    addManagerTreeNodeToRootNode(idString.str().c_str(), levelNodeData);
    loadLevelMeshDialogIsShown = false;
}

void ApplicationDelegate::quit() {
    device->closeDevice();
}

irr::gui::IGUITreeView* ApplicationDelegate::getGameTreeView() {
    return reinterpret_cast<irr::gui::IGUITreeView*>(guienv->getRootGUIElement()->getElementFromId(static_cast<irr::s32>(GuiElementID::GAME_LEVEL_TREE), true));
}

void ApplicationDelegate::addManagerTreeNodeToRootNode(std::wstring label, GameManagerNodeData* nodeData) {
    irr::gui::IGUITreeView* tree = getGameTreeView();
    tree->getRoot()->addChildBack(label.c_str(), nullptr, -1, -1, reinterpret_cast<void*>(nodeData));
}

void ApplicationDelegate::addManagerTreeNodeToSelectedNode(std::wstring label, GameManagerNodeData* nodeData) {
    irr::gui::IGUITreeView* tree = getGameTreeView();
    irr::gui::IGUITreeViewNode* selectedNode = tree->getSelected();

    if (!selectedNode) {
        selectedNode = tree->getRoot();
    }

    selectedNode->addChildBack(label.c_str(), nullptr, -1, -1, nodeData);
}

void ApplicationDelegate::setFont() {
    irr::gui::IGUIFont* font = guienv->getFont("Resources/Fonts/calibri.xml");
    guienv->getSkin()->setFont(font);
}
