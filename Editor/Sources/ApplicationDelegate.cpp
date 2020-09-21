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
    loadLevelMeshDialogIsShown(false),
    currentLevel(nullptr),
    currentTarget(nullptr),
    gameData(std::make_shared<GameData>(device))
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
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::LOAD_LEVELS), nullptr, L"Load game levels", openFileIcon, nullptr, false, true);

    irr::video::ITexture* saveFileIcon = driver->getTexture("Resources/Icons/save.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::SAVE_LEVELS), nullptr, L"Save game levels", saveFileIcon, nullptr, false, true);

    irr::video::ITexture* helpIcon = driver->getTexture("Resources/Icons/help.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::ABOUT), nullptr, L"About", helpIcon, nullptr, false, true);

    irr::video::ITexture* addLevelIcon = driver->getTexture("Resources/Icons/map.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::ADD_LEVEL), nullptr, L"Add level", addLevelIcon, nullptr, false, true);

    irr::video::ITexture* addTargetIcon = driver->getTexture("Resources/Icons/map-pin.png");
    toolbar->addButton(static_cast<irr::s32>(GuiElementID::ADD_TARGET), nullptr, L"Add target", addTargetIcon, nullptr, false, true);
}

void ApplicationDelegate::createManagerWindow() {
    irr::gui::IGUIWindow* managerWindow = guienv->addWindow(
        irr::core::rect<irr::s32>(600, 70, 800, 470),
        false, 
        L"Levels manager",
        nullptr,
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
    if (currentLevel == nullptr) {
        guienv->addMessageBox(L"Error", L"You have to have at least one level before placing a target");
        return;
    }

    irr::core::vector3df targetPosition = getTargetPositionFromCameraView();

    size_t targetIndex = currentLevel->getTargets().size();
    std::wostringstream idString;
    idString << currentLevel->getId() << "-target-" << targetIndex;

    currentLevel->addTargetPosition(targetPosition, idString.str());

    rebuildGameManagerTree();

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
    gameData->saveToFile(filename);

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
    gameData->loadFromFile(filename);

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
    std::shared_ptr<Level> level = gameData->createLevel(meshFilename);

    rebuildGameManagerTree();

    loadLevelMeshDialogIsShown = false;
}

void ApplicationDelegate::quit() {
    device->closeDevice();
}

irr::gui::IGUITreeView* ApplicationDelegate::getGameTreeView() {
    return reinterpret_cast<irr::gui::IGUITreeView*>(guienv->getRootGUIElement()->getElementFromId(static_cast<irr::s32>(GuiElementID::GAME_LEVEL_TREE), true));
}

irr::gui::IGUITreeViewNode* ApplicationDelegate::addManagerTreeNodeToRootNode(std::wstring label, GameManagerNodeData* nodeData) {
    irr::gui::IGUITreeView* tree = getGameTreeView();

    return addManagerTreeNodeToNode(label, nodeData, tree->getRoot());
}

irr::gui::IGUITreeViewNode* ApplicationDelegate::addManagerTreeNodeToSelectedNode(std::wstring label, GameManagerNodeData* nodeData) {
    irr::gui::IGUITreeView* tree = getGameTreeView();
    irr::gui::IGUITreeViewNode* selectedNode = tree->getSelected();

    if (!selectedNode) {
        selectedNode = tree->getRoot();
    }

    return addManagerTreeNodeToNode(label, nodeData, selectedNode);
}

irr::gui::IGUITreeViewNode* ApplicationDelegate::addManagerTreeNodeToNode(std::wstring label, GameManagerNodeData* nodeData, irr::gui::IGUITreeViewNode* parent) {
    return parent->addChildBack(label.c_str(), nullptr, -1, -1, reinterpret_cast<void*>(nodeData));
}

void ApplicationDelegate::setFont() {
    irr::gui::IGUIFont* font = guienv->getFont("Resources/Fonts/calibri.xml");
    guienv->getSkin()->setFont(font);
}

void ApplicationDelegate::levelSelected(const std::wstring& levelId) {
    currentLevel = gameData->getLevelById(levelId);

    // TODO: unload current level
    // TODO: load level
}

void ApplicationDelegate::targetSelected(const std::wstring& targetId) {
    if (currentLevel == nullptr) {
        return;
    }

    currentTarget = currentLevel->getTargetById(targetId);

    // TODO: additional behavior
}

void ApplicationDelegate::gameManagerNodeSelected() {
    irr::gui::IGUITreeView* gameManager = getGameTreeView();
    irr::gui::IGUITreeViewNode* selectedNode = gameManager->getSelected();

    GameManagerNodeData* nodeData = reinterpret_cast<GameManagerNodeData*>(selectedNode->getData());

    if (nodeData->getType() == GameManagerNodeDataType::LEVEL) {
        levelSelected(nodeData->getId());
    }
    else {
        targetSelected(nodeData->getId());
    }
}

irr::core::vector3df ApplicationDelegate::getTargetPositionFromCameraView() const {
    irr::core::triangle3df triangle;
    irr::core::vector3df collisionPoint;
    irr::core::line3df ray(camera->getAbsolutePosition(), camera->getAbsolutePosition() + camera->getTarget() * camera->getFarValue());

    const float PICK_DISTANCE = 20.f;

    irr::scene::ISceneNode* collisionNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collisionPoint, triangle);

    if (collisionNode) {
        return collisionPoint;
    }

    irr::core::vector3df targetPosition = camera->getAbsolutePosition() + camera->getTarget() * PICK_DISTANCE;

    return targetPosition;
}

void ApplicationDelegate::rebuildGameManagerTree() {
    auto gameManagerTree = getGameTreeView();

    gameManagerTree->getRoot()->clearChildren();

    for (auto level : gameData->getLevels()) {
        GameManagerNodeData* levelNodeData = new GameManagerNodeData(GameManagerNodeDataType::LEVEL, level->getId());

        addManagerTreeNodeToRootNode(level->getMeshBasename().c_str(), levelNodeData);

        for (auto target : level->getTargets()) {
            GameManagerNodeData* targetNodeData = new GameManagerNodeData(GameManagerNodeDataType::TARGET, target->getId());

            addManagerTreeNodeToRootNode(target->getId(), targetNodeData);
        }
    }
}
