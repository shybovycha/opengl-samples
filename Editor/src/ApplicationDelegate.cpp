#include "ApplicationDelegate.h"

ApplicationDelegate::ApplicationDelegate(irr::IrrlichtDevice* _device) :
        device(_device),
        smgr(device->getSceneManager()),
        guienv(device->getGUIEnvironment()),
        driver(device->getVideoDriver()),
        camera(nullptr),
        loadLevelsDialogIsShown(false),
        saveLevelsDialogIsShown(false),
        aboutWindowIsShown(false),
        loadLevelMeshDialogIsShown(false),
        arrowsParentNode(nullptr),
        triangleSelector(nullptr)
{
    gameData = std::make_shared<GameData>(device);
    gameManagerTree = std::make_shared<GameManagerTree>(guienv, gameData);
}

void ApplicationDelegate::init()
{
    camera = smgr->addCameraSceneNode();

    auto animator = new CameraSceneNodeAnimator(device->getCursorControl());
    camera->addAnimator(animator);

    createAxis();

    lightBillboardTexture = driver->getTexture("resources/icons/idea.png");

    initUI();
}

void ApplicationDelegate::initUI()
{
    setFont();

    createToolbar();

    createManagerWindow();

    gameManagerTree->init();
}

void ApplicationDelegate::createAxis()
{
    arrowsParentNode = smgr->addEmptySceneNode();
    arrowsParentNode->grab();

    auto xArrowMesh = smgr->getGeometryCreator()->createArrowMesh(4, 8, 25.f, 20.0f, 0.3f, 1.f,
            irr::video::SColor(255, 255, 0, 0));
    xArrowNode = smgr->addMeshSceneNode(xArrowMesh, arrowsParentNode, -1, irr::core::vector3df(0, 0, 0),
            irr::core::vector3df(0, 0, -90));
    xArrowNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);

    auto yArrowMesh = smgr->getGeometryCreator()->createArrowMesh(4, 8, 25.f, 20.f, 0.3f, 1.f,
            irr::video::SColor(255, 0, 255, 0));
    yArrowNode = smgr->addMeshSceneNode(yArrowMesh, arrowsParentNode, -1, irr::core::vector3df(0, 0, 0),
            irr::core::vector3df(0, 0, 0));
    yArrowNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);

    auto zArrowMesh = smgr->getGeometryCreator()->createArrowMesh(4, 8, 25.f, 20.f, 0.3f, 1.f,
            irr::video::SColor(255, 0, 0, 255));
    zArrowNode = smgr->addMeshSceneNode(zArrowMesh, arrowsParentNode, -1, irr::core::vector3df(0, 0, 0),
            irr::core::vector3df(90, 0, 0));
    zArrowNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);

    arrowsParentNode->setVisible(false);

    auto metaTriangleSelector = smgr->createMetaTriangleSelector();

    metaTriangleSelector->addTriangleSelector(smgr->createTriangleSelector(xArrowMesh, xArrowNode));
    metaTriangleSelector->addTriangleSelector(smgr->createTriangleSelector(yArrowMesh, yArrowNode));
    metaTriangleSelector->addTriangleSelector(smgr->createTriangleSelector(zArrowMesh, zArrowNode));

    triangleSelector = metaTriangleSelector;
}

void ApplicationDelegate::createToolbar()
{
    irr::gui::IGUIToolBar* toolbar = guienv->addToolBar();
    toolbar->setMinSize(irr::core::dimension2du(100, 40));

    irr::video::ITexture* openFileIcon = driver->getTexture("resources/icons/opened-folder.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::LOAD_LEVELS), nullptr, L"Load game levels", openFileIcon,
            nullptr, false, true);

    irr::video::ITexture* saveFileIcon = driver->getTexture("resources/icons/save.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::SAVE_LEVELS), nullptr, L"Save game levels", saveFileIcon,
            nullptr, false, true);

    irr::video::ITexture* helpIcon = driver->getTexture("resources/icons/help.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ABOUT), nullptr, L"About", helpIcon, nullptr, false, true);

    irr::video::ITexture* addLevelIcon = driver->getTexture("resources/icons/map.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ADD_LEVEL), nullptr, L"Add level", addLevelIcon, nullptr,
            false, true);

    irr::video::ITexture* addTargetIcon = driver->getTexture("resources/icons/accuracy.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ADD_TARGET), nullptr, L"Add target", addTargetIcon, nullptr,
            false, true);

    irr::video::ITexture* addLightIcon = driver->getTexture("resources/icons/light-on.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ADD_LIGHT), nullptr, L"Add light", addLightIcon, nullptr,
            false, true);

    irr::video::ITexture* deleteSelectedIcon = driver->getTexture("resources/icons/delete-forever.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::DELETE_SELECTED), nullptr, L"Delete selected object",
            deleteSelectedIcon, nullptr, false, true);
}

void ApplicationDelegate::createManagerWindow()
{
    irr::gui::IGUIWindow* managerWindow = guienv->addWindow(
            irr::core::rect<irr::s32>(600, 70, 800, 470),
            false,
            L"Levels manager",
            nullptr,
            static_cast<irr::s32>(GUIElementId::MANAGER_WINDOW)
    );

    managerWindow->getCloseButton()->remove();

    guienv->addTreeView(
            irr::core::rect<irr::s32>(10, 30, 190, 390),
            managerWindow,
            static_cast<irr::s32>(GUIElementId::GAME_LEVEL_TREE)
    );
}

void ApplicationDelegate::update()
{
    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    smgr->drawAll();
    arrowsParentNode->render();

    guienv->drawAll();

    driver->endScene();
}

void ApplicationDelegate::quit()
{
    device->closeDevice();
}

void ApplicationDelegate::setFont()
{
    irr::gui::IGUIFont* font = guienv->getFont("resources/Fonts/calibri.xml");
    guienv->getSkin()->setFont(font);
}

void ApplicationDelegate::openSaveLevelsDialog()
{
    if (saveLevelsDialogIsShown)
    {
        return;
    }

    // guienv->addFileOpenDialog(L"Save levels file", true, nullptr, static_cast<irr::s32>(GUIElementId::SAVE_LEVELS_DIALOG));
    auto saveFileDialog = new SaveFileDialog(L"Save levels file", guienv, nullptr,
            static_cast<irr::s32>(GUIElementId::SAVE_LEVELS_DIALOG), true);
    guienv->getRootGUIElement()->addChild(saveFileDialog);
    saveLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeSaveLevelsDialog()
{
    saveLevelsDialogIsShown = false;
}

void ApplicationDelegate::openLoadLevelsDialog()
{
    if (loadLevelsDialogIsShown)
    {
        return;
    }

    guienv->addFileOpenDialog(L"Load levels file", true, nullptr,
            static_cast<irr::s32>(GUIElementId::LOAD_LEVELS_DIALOG));
    loadLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeLoadLevelsDialog()
{
    loadLevelsDialogIsShown = false;
}

void ApplicationDelegate::openAboutWindow()
{
    if (aboutWindowIsShown)
    {
        return;
    }

    const wchar_t* ABOUT_TEXT = L"This is the ShootThem! level editor.\n\n\
Use the toolbar at the top to load level models, add targets and lights to the level.\n\
You can also delete the currently selected (in the game tree, on the right) light, \n\
target or level using the Delete button from the toolbar.\n\n\
Use Right mouse button to move camera around, Middle mouse button to move camera left/right/up/down.\n\
Shift + Middle mouse button to move camera back and forward.\n\n\
Save and load levels from the levels.xml file that will be picked up by the game later on.";

    guienv->addMessageBox(L"About", ABOUT_TEXT);

    aboutWindowIsShown = true;
}

void ApplicationDelegate::closeAboutWindow()
{
    aboutWindowIsShown = false;
}

void ApplicationDelegate::openLoadLevelMeshDialog()
{
    if (loadLevelMeshDialogIsShown)
    {
        return;
    }

    guienv->addFileOpenDialog(L"Load level mesh", true, nullptr,
            static_cast<irr::s32>(GUIElementId::LOAD_LEVEL_MESH_DIALOG));

    loadLevelMeshDialogIsShown = true;
}

void ApplicationDelegate::closeLoadLevelMeshDialog()
{
    loadLevelMeshDialogIsShown = false;
}

void ApplicationDelegate::loadLevels(const std::wstring& filename)
{
    gameData->loadFromFile(filename);

    if (!gameData->getLevels().empty())
    {
        levelSelected(gameData->getLevels().at(0)->getId());
    }

    gameManagerTree->rebuild();

    loadLevelsDialogIsShown = false;
}

void ApplicationDelegate::saveLevels()
{
    openSaveLevelsDialog();
}

void ApplicationDelegate::saveLevels(const std::wstring& filename)
{
    loadLevelsDialogIsShown = false;

    gameData->saveToFile(filename);
}

void ApplicationDelegate::addLevel(const std::wstring& meshFilename)
{
    loadLevelMeshDialogIsShown = false;

    irr::scene::ISceneNode* sceneNode = loadMesh(meshFilename);

    if (!sceneNode)
    {
        return;
    }

    std::shared_ptr<Level> level = gameData->createLevel(meshFilename);

    level->setSceneNode(sceneNode);

    levelSelected(level->getId());

    gameManagerTree->rebuild();
}

void ApplicationDelegate::addTarget()
{
    if (gameData->getCurrentLevel() == nullptr)
    {
        guienv->addMessageBox(L"Error", L"You have to select a level before placing a target");
        return;
    }

    irr::core::vector3df targetPosition = getTargetPositionFromCameraView();

    std::shared_ptr<Target> target = gameData->getCurrentLevel()->createTarget(targetPosition);

    // TODO: replace with actual target model
    irr::scene::ISceneNode* targetSceneNode = smgr->addSphereSceneNode(5.0f, 16, nullptr, -1, targetPosition);

    target->setSceneNode(targetSceneNode);

    targetSelected(target->getId());

    gameManagerTree->rebuild();
}

void ApplicationDelegate::addLight()
{
    if (gameData->getCurrentLevel() == nullptr)
    {
        guienv->addMessageBox(L"Error", L"You have to select a level before placing a light");
        return;
    }

    irr::core::vector3df lightPosition = getTargetPositionFromCameraView();

    std::shared_ptr<Light> light = gameData->getCurrentLevel()->createLight(lightPosition);

    irr::scene::ISceneNode* lightParentSceneNode = smgr->addEmptySceneNode();
    lightParentSceneNode->grab();
    lightParentSceneNode->setPosition(lightPosition);

    irr::scene::ILightSceneNode* lightSceneNode = smgr->addLightSceneNode(lightParentSceneNode,
            irr::core::vector3df(0, 0, 0), irr::video::SColor(255, 255, 255, 255), 200.f);
    light->setSceneNode(lightParentSceneNode);

    irr::scene::IBillboardSceneNode* lightIconSceneNode = smgr->addBillboardSceneNode(lightParentSceneNode,
            irr::core::dimension2df(8, 8));
    lightIconSceneNode->setMaterialTexture(0, lightBillboardTexture);
    lightIconSceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    lightIconSceneNode->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);

    lightSceneNode->setLightType(irr::video::ELT_POINT);

    lightSelected(light->getId());

    gameManagerTree->rebuild();
}

void ApplicationDelegate::levelSelected(const std::wstring& levelId)
{
    for (const auto& level : gameData->getLevels())
    {
        if (level->getSceneNode() != nullptr)
        {
            level->getSceneNode()->setVisible(false);
        }

        for (const auto& entity : level->getEntities())
        {
            if (entity->getSceneNode() != nullptr)
            {
                entity->getSceneNode()->setVisible(false);
            }
        }
    }

    gameData->setCurrentLevel(gameData->getLevelById(levelId));
    gameData->setCurrentEntity(nullptr);

    if (!gameData->getCurrentLevel()->getSceneNode())
    {
        irr::scene::ISceneNode* sceneNode = loadMesh(gameData->getCurrentLevel()->getMeshFilename());

        if (!sceneNode)
        {
            return;
        }

        gameData->getCurrentLevel()->setSceneNode(sceneNode);
    }

    gameData->getCurrentLevel()->getSceneNode()->setVisible(true);

    for (const auto& entity : gameData->getCurrentLevel()->getEntities())
    {
        if (entity->getSceneNode() != nullptr)
        {
            entity->getSceneNode()->setVisible(true);
        }
    }

    arrowsParentNode->setParent(nullptr);
    arrowsParentNode->setVisible(false);
}

void ApplicationDelegate::targetSelected(const std::wstring& targetId)
{
    if (gameData->getCurrentLevel() == nullptr)
    {
        return;
    }

    auto target = gameData->getCurrentLevel()->getEntityById(targetId);

    gameData->setCurrentEntity(target);

    arrowsParentNode->setParent(target->getSceneNode());
    arrowsParentNode->setVisible(true);

    // TODO: additional behavior
}

void ApplicationDelegate::lightSelected(const std::wstring& lightId)
{
    if (gameData->getCurrentLevel() == nullptr)
    {
        return;
    }

    auto light = gameData->getCurrentLevel()->getEntityById(lightId);

    gameData->setCurrentEntity(light);

    arrowsParentNode->setParent(light->getSceneNode());
    arrowsParentNode->setVisible(true);

    // TODO: additional behavior
}

void ApplicationDelegate::gameManagerNodeSelected()
{
    GameManagerNodeData* nodeData = gameManagerTree->getSelectedNodeData();

    if (!nodeData)
    {
        return;
    }

    if (nodeData->getType() == GameManagerNodeDataType::LEVEL)
    {
        levelSelected(nodeData->getId());
    }
    else
    {
        targetSelected(nodeData->getId());
    }
}

void ApplicationDelegate::deleteSelectedEntity()
{
    auto currentLevel = gameData->getCurrentLevel();
    auto currentEntity = gameData->getCurrentEntity();

    if (currentLevel != nullptr && currentEntity == nullptr)
    {
        std::vector<std::shared_ptr<LevelEntity>> entities = currentLevel->getEntities();

        for (auto const& it : entities)
        {
            currentLevel->deleteEntityById(it->getId());
        }

        gameData->deleteLevelById(currentLevel->getId());
        gameData->setCurrentLevel(nullptr);
        gameManagerTree->rebuild();
    }
    else if (currentEntity != nullptr)
    {
        currentLevel->deleteEntityById(currentEntity->getId());
        gameData->setCurrentEntity(nullptr);
        gameManagerTree->rebuild();
    }
}

irr::core::vector3df ApplicationDelegate::getTargetPositionFromCameraView() const
{
    // ray picking does not really work =(
    /*irr::core::triangle3df triangle;
    irr::core::vector3df collisionPoint;
    irr::core::line3df ray(camera->getPosition(), camera->getPosition() + camera->getTarget() * camera->getFarValue());

    irr::scene::ISceneNode* collisionNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collisionPoint, triangle);

    if (collisionNode) {
        return collisionPoint;
    }*/

    const float PICK_DISTANCE = 200.f;

    irr::core::vector3df target = (camera->getTarget() - camera->getPosition()).normalize();

    irr::core::vector3df targetPosition = camera->getPosition() + target * PICK_DISTANCE;

    return targetPosition;
}

irr::scene::ISceneNode* ApplicationDelegate::loadMesh(const std::wstring& meshFilename)
{
    irr::scene::IMesh* levelMesh = smgr->getMesh(meshFilename.c_str());

    if (!levelMesh)
    {
        std::wostringstream errorMessage;
        errorMessage << "Could not load level mesh file: " << meshFilename;
        guienv->addMessageBox(L"Error", errorMessage.str().c_str());
        return nullptr;
    }

    irr::scene::ISceneNode* sceneNode = smgr->addMeshSceneNode(levelMesh);
    sceneNode->setName(meshFilename.c_str());
    return sceneNode;
}

void ApplicationDelegate::updateSelectedNodeMovement(bool isLeftMouseButtonDown)
{
    auto mousePosition = device->getCursorControl()->getPosition();
    auto mousePositionDelta = mousePosition - previousMousePosition;

    previousMousePosition = mousePosition;

    auto currentEntity = gameData->getCurrentEntity();

    if (currentEntity == nullptr)
    {
        return;
    }

    auto selectedSceneNode = smgr->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB(mousePosition);

    if (selectedSceneNode == nullptr)
    {
        return;
    }

    if (!isLeftMouseButtonDown)
    {
        xArrowNode->setVisible(true);
        yArrowNode->setVisible(true);
        zArrowNode->setVisible(true);

        selectedEntityMoveDirection = std::nullopt;

        return;
    }

    irr::core::vector3df direction(0, 0, 0);

    if (selectedEntityMoveDirection.has_value())
    {
        direction = selectedEntityMoveDirection.value();
    }
    else
    {
        if (selectedSceneNode == xArrowNode)
        {
            yArrowNode->setVisible(false);
            zArrowNode->setVisible(false);

            direction = irr::core::vector3df(1, 0, 0);
        }
        else if (selectedSceneNode == yArrowNode)
        {
            xArrowNode->setVisible(false);
            zArrowNode->setVisible(false);

            direction = irr::core::vector3df(0, 1, 0);
        }
        else if (selectedSceneNode == zArrowNode)
        {
            xArrowNode->setVisible(false);
            yArrowNode->setVisible(false);

            direction = irr::core::vector3df(0, 0, 1);
        }
        else
        {
            xArrowNode->setVisible(true);
            yArrowNode->setVisible(true);
            zArrowNode->setVisible(true);

            selectedEntityMoveDirection = std::nullopt;

            return;
        }

        selectedEntityMoveDirection = direction;
    }

    auto distanceToCamera = (currentEntity->getSceneNode()->getPosition() - camera->getPosition()).getLength();
    auto offset = mousePositionDelta.getLength();

    // figure out the offset sign coefficient based on cos((VIEW_MATRIX * (entityPosition + direction)), mousePositionDelta)
    // e.g. the angle (> 90 deg or < 90 deg) between the direction from the entity position, as projected onto a screen AND mouse offset vector
    auto entityPosition = currentEntity->getPosition();
    auto entityPositionNext = currentEntity->getPosition() + direction;
    auto entityPositionOnScreen = smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(entityPosition);
    auto entityPositionOnScreenNext = smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(
            entityPositionNext);
    auto screenEntityDirection = entityPositionOnScreenNext - entityPositionOnScreen;
    auto dotProduct = screenEntityDirection.dotProduct(mousePositionDelta);
    auto sign = (dotProduct > 0) ? 1 : -1;

    auto newPosition = currentEntity->getPosition() + direction * distanceToCamera * offset / 100.f * sign;

    currentEntity->setPosition(newPosition);
}
