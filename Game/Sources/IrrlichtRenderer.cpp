#include "IrrlichtRenderer.h"

IrrlichtRenderer::IrrlichtRenderer(std::shared_ptr<GameState> _gameState) : Renderer(std::move(_gameState)) {}

void IrrlichtRenderer::init(Settings settings) {
    irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_OPENGL;

    if (settings.driverName == "DirectX") {
        driverType = irr::video::EDT_DIRECT3D9;
    }

    irr::core::dimension2d<irr::u32> resolution = irr::core::dimension2d<irr::u32>(settings.resolutionWidth, settings.resolutionHeight);

    device = std::shared_ptr<irr::IrrlichtDevice>(irr::createDevice(driverType, resolution, settings.colorDepth, settings.fullScreen, settings.stencil, settings.vsync));

    device->setWindowCaption(L"ShootThem!");

    driver = std::shared_ptr<irr::video::IVideoDriver>(device->getVideoDriver());
    smgr = std::shared_ptr<irr::scene::ISceneManager>(device->getSceneManager());
    guienv = std::shared_ptr<irr::gui::IGUIEnvironment>(device->getGUIEnvironment());

    soundEngine = std::shared_ptr<irrklang::ISoundEngine>(irrklang::createIrrKlangDevice());

    device->getFileSystem()->addZipFileArchive("Resources/Packs/data.pk3");

    bill = smgr->addBillboardSceneNode();
    bill->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
    bill->setMaterialTexture(0, driver->getTexture("cross.bmp"));
    bill->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    bill->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    bill->setSize(irr::core::dimension2d<irr::f32>(20.0f, 20.0f));

    // TODO: move this to scene config too
    smgr->addLightSceneNode(0, irr::core::vector3df(0, 20, 0), irr::video::SColorf(0.5f, 0.5f, 0.5f, 0.5f), 3000, 0);

    // driver->setFog(irr::video::SColor(0, 138, 125, 81), irr::video::EFT_FOG_LINEAR, 250, 1000, 0, true);

    // TODO: move this to scene config too as a player initial position
    camera = std::shared_ptr<irr::scene::ICameraSceneNode>(smgr->addCameraSceneNodeFPS(0, 100, 0, 0));
    device->getCursorControl()->setVisible(false);

    statusBar = guienv->addStaticText(L"New game", irr::core::rect<irr::s32>(10, 10, 260, 22), true, true, 0, 0, true);
    timer = device->getTimer();
    timer->start();

    playermesh = smgr->getMesh("rifle.3ds");
    player = smgr->addAnimatedMeshSceneNode(playermesh);

    // TODO: player model offset relative to camera position
    player->setPosition(irr::core::vector3df(0.5f, -1.0f, 1.0f));

    player->setParent(camera.get());

    // TODO: should these be initialized here???
    inputHandler = std::make_shared<InputHandler>(gameState);
    eventReceiver = std::make_shared<IrrlichtEventReceiver>(inputHandler, smgr, camera);

    device->setEventReceiver(eventReceiver.get());

    // TODO: maybe have some style?
    gameState->enqueue(new LoadFirstLevelAction(gameState->getCurrentLevel()));
}

void IrrlichtRenderer::processActionQueue() {
    while (gameState->hasActions()) {
        auto action = gameState->nextAction();

        switch (action->getType()) {
        case QueueActionType::LOAD_FIRST_LEVEL:
            processAction(reinterpret_cast<LoadFirstLevelAction*>(action));
            break;
        case QueueActionType::LOAD_NEXT_LEVEL:
            processAction(reinterpret_cast<LoadNextLevelAction*>(action));
            break;
        case QueueActionType::PLAY_SOUND:
            processAction(reinterpret_cast<PlaySoundAction*>(action));
            break;
        case QueueActionType::TARGET_ELIMINATED:
            processAction(reinterpret_cast<TargetEliminatedAction*>(action));
            break;
        }
    }
}

void IrrlichtRenderer::processAction(PlaySoundAction* action) {
    soundEngine->play2D(action->getSoundFile().c_str(), false);
}

void IrrlichtRenderer::processAction(LoadFirstLevelAction* action) {
    irr::scene::IAnimatedMesh* levelMesh = smgr->getMesh(action->getLevel()->getModelFilename().c_str());

    std::shared_ptr<irr::scene::IAnimatedMeshSceneNode> level(smgr->addAnimatedMeshSceneNode(levelMesh));

    action->getLevel()->setModel(level);

    selector = std::shared_ptr<irr::scene::ITriangleSelector>(smgr->createOctTreeTriangleSelector(levelMesh->getMesh(0), level.get(), 128));

    irr::scene::IAnimatedMesh* targetMesh = smgr->getMesh("chicken.3ds");
    std::vector<std::shared_ptr<irr::scene::ISceneNode>> targets;

    for (auto position : action->getLevel()->getTargetPositions()) {
        std::shared_ptr<irr::scene::ISceneNode> target(smgr->addAnimatedMeshSceneNode(targetMesh));

        target->setVisible(true);

        target->setMaterialTexture(0, driver->getTexture("Chick02.bmp"));
        target->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);
        target->setPosition(position);

        targets.push_back(std::move(target));
    }

    action->getLevel()->setTargets(targets);
}

void IrrlichtRenderer::processAction(LoadNextLevelAction* action) {
    // unload existing level data
    // TODO: ISceneNode::drop() does not work here for some reason. Neither ISceneNode::remove() does
    action->getPreviousLevel()->getModel()->setVisible(false);

    for (auto target : action->getPreviousLevel()->getTargets()) {
        target->setVisible(false);
    }

    // load next level
    irr::scene::IAnimatedMesh* levelMesh = smgr->getMesh(action->getNextLevel()->getModelFilename().c_str());

    std::shared_ptr<irr::scene::IAnimatedMeshSceneNode> level(smgr->addAnimatedMeshSceneNode(levelMesh));

    action->getNextLevel()->setModel(level);

    selector = std::shared_ptr<irr::scene::ITriangleSelector>(smgr->createOctTreeTriangleSelector(levelMesh->getMesh(0), level.get(), 128));

    irr::scene::IAnimatedMesh* targetMesh = smgr->getMesh("chicken.3ds");

    std::vector<std::shared_ptr<irr::scene::ISceneNode>> targets;

    for (auto position : action->getNextLevel()->getTargetPositions()) {
        std::shared_ptr<irr::scene::ISceneNode> target(smgr->addAnimatedMeshSceneNode(targetMesh));

        target->setVisible(true);

        target->setMaterialTexture(0, driver->getTexture("Chick02.bmp"));
        target->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);
        target->setPosition(position);

        targets.push_back(std::move(target));
    }

    action->getNextLevel()->setTargets(targets);
    gameState->nextLevelLoaded();
    gameState->getCurrentScore()->resetTargetEliminated();
}

void IrrlichtRenderer::processAction(TargetEliminatedAction* action) {
    action->getTarget()->setVisible(false);
    gameState->getCurrentScore()->targetEliminated();

    if (gameState->getCurrentScore()->getTargetsEliminated() >= gameState->getCurrentLevel()->getTargets().size()) {
        // TODO: show next level menu
        gameState->enqueue(new LoadNextLevelAction(gameState->getCurrentLevel(), gameState->getNextLevel()));
    }
}

void IrrlichtRenderer::render() {
    // TODO: here should the menu logic be
    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    smgr->drawAll();
    guienv->drawAll();

    updateStatusBar();

    driver->endScene();
}

void IrrlichtRenderer::shutdown() {
    device->drop();
    soundEngine->drop();

    timer->stop();
}

bool IrrlichtRenderer::isRunning() {
    // TODO: add (main menu) state checks
    return device->run();
}

// TODO: this is the endgame screen
void IrrlichtRenderer::showResult() {
    irr::core::stringw title = L"Level complete!";

    int points = gameState->getCurrentScore()->getTargetsEliminated();
    int targetCnt = gameState->getCurrentLevel()->getTargets().size();

    // int shots = gameState->getCurrentScore()->getShots();
    int shots = 0;

    std::wostringstream msg;
    msg << "Your time: " << (MAX_TIME / 100) - abs(Tm / 100) << "sec;  Shots: " << shots << "/" << targetCnt << " min" << ";  Target hit: " << points << "/" << targetCnt;

    /*Tms += (MAX_TIME / 100) - abs(Tm / 100);
    Pnts += points;*/

    /*guienv->addMessageBox(title.c_str(), msg.str().c_str(), true, irr::gui::EMBF_OK, 0, 0);

    endLevel = true;

    timer->stop();

    if (levelNumber + 1 < levelCnt) {
        return;
    }

    guienv->getSkin()->setFont(guienv->getFont("fontcourier.bmp"));
    guienv->clear();
    guienv->addMessageBox(L"Congratulations!", L"Game over!", true, irr::gui::EMBF_OK, 0, 0);*/
}

void IrrlichtRenderer::updateStatusBar() {
    int points = gameState->getCurrentScore()->getTargetsEliminated();
    int targetCnt = gameState->getCurrentLevel()->getTargets().size();

    int ammo = gameState->getPlayerState()->getCurrentAmmo();
    int maxAmmo = gameState->getPlayerState()->getMaxAmmo();

    int levelIdx = gameState->getCurrentLevelIndex();
    int levelsCnt = gameState->getLevelsCnt();

    std::wostringstream statusString;

    statusString << "Ammo: " << ammo << "/" << maxAmmo << "; Points: " << points << "/" << targetCnt << "; Time:" << Tm / 100 << "; Level:" << levelIdx + 1 << "/" << levelsCnt;

    if (!timer->isStopped() && levelIdx + 1 < levelsCnt) {
        Tm--;
    }

    /*if ((Tm <= 0 || points == targetCnt) && (endLevel == false)) {
        showResult();
    }*/

    statusBar->setText(statusString.str().c_str());

    float k = (sin(abs(Tm) / 100) / (10 - gameState->getCurrentLevelIndex()));

    camera->setRotation(
        irr::core::vector3df(
            camera->getRotation().X + k,
            camera->getRotation().Y,
            camera->getRotation().Z
        )
    );

    irr::core::line3d<irr::f32> line;
    line.start = camera->getPosition();
    line.end = line.start + (camera->getTarget() - line.start).normalize() * 10000.0f;

    irr::core::vector3df intersection;
    irr::core::triangle3df tri;

    irr::scene::ISceneNode* node = 0;

    if (smgr->getSceneCollisionManager()->getCollisionPoint(line, selector.get(), intersection, tri, node)) {
        bill->setPosition(intersection);
    }
}
