#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <memory>
#include <string>

#include <irrlicht.h>
#include <irrKlang.h>
#include <tinyxml2.h>

#pragma comment(lib, "Irrlicht.lib")

/*
    --------------------------------------------

    This part of code is modern, written with few best practices in mind.
    It also introduces some significant changes to the game architecture (like game state, 
    loading levels from files, bringing some sane format for game resources files, etc.).
    This is essentially a new version of the game, taking into consideration all the knowledge
    acquired in last 13 years.
    Feel free to blame the author for beating Web for past decade and the global crisis(-es)
    going on in the world outside if you find this code shit.

    (c) Artem Shubovych, 10 September 2020.

    --------------------------------------------
*/

struct Settings {
    std::string driverName;
    int resolutionWidth;
    int resolutionHeight;
    int colorDepth;
    bool fullScreen;
    bool vsync;
    bool stencil;
};

class Level {
public:
    Level(const std::string filename) : meshFilename(filename) {}

    const std::string getModelFilename() const {
        return meshFilename;
    }

    void addTargetPosition(irr::core::vector3df position) {
        targetPositions.push_back(position);
    }

    const std::vector<irr::core::vector3df> getTargetPositions() const {
        return targetPositions;
    }

    void setModel(std::shared_ptr<irr::scene::ISceneNode> mesh) {
        model = mesh;
    }

    const std::shared_ptr<irr::scene::ISceneNode> getModel() const {
        return model;
    }

    const std::vector<std::shared_ptr<irr::scene::ISceneNode>> getTargets() const {
        return targets;
    }

    void setTargets(std::vector<std::shared_ptr<irr::scene::ISceneNode>> _targets) {
        targets = _targets;
    }

private:
    std::string meshFilename;

    std::vector<irr::core::vector3df> targetPositions;
    std::vector<std::shared_ptr<irr::scene::ISceneNode>> targets;

    irr::core::vector3df playerPosition;

    std::shared_ptr<irr::scene::ISceneNode> model;
};

class Score {
public:
    Score() : targetsEliminated(0), currentTime(0) {}

    void targetEliminated() {
        targetsEliminated++;
    }

    void resetTargetEliminated() {
        targetsEliminated = 0;
    }

    void timeUsed(unsigned long time) {
        currentTime += time;
    }

    void resetTimeUsed() {
        currentTime = 0;
    }

    const unsigned int getTargetsEliminated() const {
        return targetsEliminated;
    }

    const unsigned long getCurrentTime() const {
        return targetsEliminated;
    }

private:
    unsigned int targetsEliminated;
    unsigned long currentTime;
};

class PlayerState {
public:
    PlayerState() : currentAmmo(10), maxAmmo(10) {}

    void setMaxAmmo(unsigned int _maxAmmo) {
        maxAmmo = _maxAmmo;
    }

    void reload() {
        if (currentAmmo < maxAmmo) {
            currentAmmo = maxAmmo;
        }
    }

    void shoot() {
        if (currentAmmo > 0) {
            currentAmmo--;
        }
    }

    const unsigned int getCurrentAmmo() const {
        return currentAmmo;
    }

    const unsigned int getMaxAmmo() const {
        return maxAmmo;
    }

private:
    unsigned int currentAmmo;
    unsigned int maxAmmo;
};

enum class E_GAME_STATE {
    MAIN_MENU,
    PLAYING,
    END_LEVEL,
    END_GAME
};

/*
* Polymorphic method won't work:
*
* virtual void act(std::shared_ptr<Renderer> renderer) = 0
* 
* You are fine with only Renderer's public or friend API.
* But if you want to use anything outside of Renderer otherwise available inside Renderer itself (like for loading resources) - you won't be able to.
* Also, I want to keep all the engine-specific logic in one place.
*/
enum class QueueActionType {
    PLAY_SOUND,
    LOAD_FIRST_LEVEL,
    LOAD_NEXT_LEVEL,
    TARGET_ELIMINATED,
};

class QueueAction {
protected:
    QueueAction(const QueueActionType _type) : type(_type) {}

public:
    const QueueActionType getType() const {
        return type;
    }

private:
    QueueActionType type;
};

class PlaySoundAction : public QueueAction {
public:
    PlaySoundAction(std::string _soundFile) : QueueAction(QueueActionType::PLAY_SOUND), soundFile(_soundFile) {}

    const std::string getSoundFile() const {
        return soundFile;
    }

private:
    std::string soundFile;
};

class LoadNextLevelAction : public QueueAction {
public:
    LoadNextLevelAction(std::shared_ptr<Level> _previousLevel, std::shared_ptr<Level> _nextLevel) : QueueAction(QueueActionType::LOAD_NEXT_LEVEL), previousLevel(_previousLevel), nextLevel(_nextLevel) {}

    const std::shared_ptr<Level> getPreviousLevel() const { 
        return previousLevel; 
    }

    const std::shared_ptr<Level> getNextLevel() const {
        return nextLevel;
    }

private:
    std::shared_ptr<Level> previousLevel;
    std::shared_ptr<Level> nextLevel;
};

class LoadFirstLevelAction : public QueueAction {
public:
    LoadFirstLevelAction(std::shared_ptr<Level> _level) : QueueAction(QueueActionType::LOAD_FIRST_LEVEL), level(_level) {}

    const std::shared_ptr<Level> getLevel() const {
        return level;
    }

private:
    std::shared_ptr<Level> level;
};

class TargetEliminatedAction : public QueueAction {
public:
    TargetEliminatedAction(std::shared_ptr<irr::scene::ISceneNode> _target) : QueueAction(QueueActionType::TARGET_ELIMINATED), target(_target) {}

    const std::shared_ptr<irr::scene::ISceneNode> getTarget() const {
        return target;
    }

private:
    std::shared_ptr<irr::scene::ISceneNode> target;
};

class GameState {
public:
    GameState() : currentState(E_GAME_STATE::MAIN_MENU), currentScore(std::make_shared<Score>()), playerState(std::make_shared<PlayerState>()), currentLevel(0) {}

    void timeElapsed(unsigned long time) {
        currentScore->timeUsed(time);
    }

    const std::shared_ptr<Score> getCurrentScore() const {
        return currentScore;
    }

    const E_GAME_STATE getCurrentState() const {
        return currentState;
    }

    const std::shared_ptr<Level> getCurrentLevel() const {
        if (levels.empty() || currentLevel < 0 || currentLevel >= levels.size()) {
            throw "Invalid current level index";
            return nullptr;
        }

        return levels.at(currentLevel);
    }

    const std::shared_ptr<PlayerState> getPlayerState() const {
        return playerState;
    }

    const int getCurrentLevelIndex() const {
        return currentLevel;
    }

    const int getLevelsCnt() const {
        return levels.size();
    }

    void nextLevelLoaded() {
        ++currentLevel;
    }

    void enqueue(QueueAction* action) {
        actionQueue.push(action);
    }

    const bool hasActions() const {
        return actionQueue.size() > 0;
    }

    QueueAction* nextAction() {
        auto action = actionQueue.front();
        actionQueue.pop();
        return action;
    }

    void setLevels(std::vector<std::shared_ptr<Level>> _levels) {
        levels = _levels;
    }


    const std::shared_ptr<Level> getNextLevel() const
    {
        if (currentLevel + 1 >= levels.size()) {
            return nullptr;
        }

        return levels.at(currentLevel + 1);
    }


private:
    E_GAME_STATE currentState;

    std::shared_ptr<Score> currentScore;
    std::shared_ptr<PlayerState> playerState;

    std::queue<QueueAction*> actionQueue;

    std::vector<std::shared_ptr<Level>> levels;
    size_t currentLevel;
};

class InputHandler {
public:
    InputHandler(std::shared_ptr<GameState> _gameState) : gameState(std::move(_gameState)) {}

    void shootAction(irr::scene::ISceneNode* objectAtCursor) {
        if (gameState->getPlayerState()->getCurrentAmmo() <= 0) {
            gameState->enqueue(new PlaySoundAction("Resources/Sounds/noammo.wav"));
            return;
        }

        gameState->enqueue(new PlaySoundAction("Resources/Sounds/shot.wav"));
        
        gameState->getPlayerState()->shoot();

        if (objectAtCursor == gameState->getCurrentLevel()->getModel().get()) {
            return;
        }

        for (auto target: gameState->getCurrentLevel()->getTargets()) {
            if (target.get() != objectAtCursor) {
                continue;
            }

            gameState->enqueue(new TargetEliminatedAction(std::move(target)));
            gameState->enqueue(new PlaySoundAction("Resources/Sounds/bell.wav"));

            break;
        }
    }

    void reloadAction() {
        gameState->getPlayerState()->reload();
        gameState->enqueue(new PlaySoundAction("Resources/Sounds/reload.wav"));
    }

    void mainMenuAction() {
        // TODO: implement properly
        exit(0);
    }

private:
    std::shared_ptr<GameState> gameState;
};

class IrrlichtEventReceiver : public irr::IEventReceiver {
public:
    IrrlichtEventReceiver(std::shared_ptr<InputHandler> _inputHandler, std::shared_ptr<irr::scene::ISceneManager> _sceneManager, std::shared_ptr<irr::scene::ICameraSceneNode> _camera) :
        inputHandler(std::move(_inputHandler)),
        sceneManager(std::move(_sceneManager)),
        camera(std::move(_camera))
    {}

    virtual bool OnEvent(const irr::SEvent& event) {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
                irr::scene::ISceneNode* objectAtCursor = sceneManager->getSceneCollisionManager()->getSceneNodeFromCameraBB(camera.get());

                inputHandler->shootAction(objectAtCursor);
            }
            else if (event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN) {
                inputHandler->reloadAction();
            }
        }
        else if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (event.KeyInput.Key == irr::KEY_ESCAPE) {
                inputHandler->mainMenuAction();
            }
        }

        /*if (event.EventType == irr::EET_GUI_EVENT) {
            if (event.GUIEvent.EventType == irr::gui::EGET_MESSAGEBOX_OK) {
                if (levelNumber + 1 == levelCnt) {
                    saveHiscores();
                    device->drop();
                    engine->drop();

                    exit(0);
                }

                if (endLevel == true) {
                    //showHiscores();

                    endLevel = false;
                    gotoMap(++levelNumber);
                    Tm = MAX_TIME;

                    return true;
                }
            }
        }

        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (event.KeyInput.Key == irr::KEY_ESCAPE) {
                saveHiscores();
                device->drop();
                engine->drop();

                exit(0);
            }

            if (event.KeyInput.Key == irr::KEY_RETURN) {
                if (hiscoremnu == true) {
                    hiscoremnu = false;
                }
            }
        }*/

        return false;
    }

private:
    std::shared_ptr<InputHandler> inputHandler;
    std::shared_ptr<irr::scene::ISceneManager> sceneManager;
    std::shared_ptr<irr::scene::ICameraSceneNode> camera;
};

class Renderer {
public:
    Renderer(std::shared_ptr<GameState> _gameState) : gameState(std::move(_gameState)) {}

    virtual void init(Settings settings) = 0;

    virtual void processActionQueue() = 0;

    virtual void render() = 0;

    virtual void shutdown() = 0;

    virtual bool isRunning() = 0;

protected:
    std::shared_ptr<GameState> gameState;
};

class ResourceManager {
public:
    ResourceManager() {}

    virtual Settings loadSettings() = 0;

    virtual std::vector<std::shared_ptr<Level>> loadLevels() = 0;
};

class ModernResourceManager : public ResourceManager {
public:
    ModernResourceManager() : ResourceManager() {}

    virtual Settings loadSettings() {
        std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();

        tinyxml2::XMLError xmlError = xml->LoadFile("Data/settings.xml");

        if (xmlError != tinyxml2::XML_SUCCESS) {
            std::cerr << "Can not load settings.xml file" << std::endl;
            throw "Can not load settings";
        }

        auto settingsNode = xml->FirstChildElement("settings");
        auto graphicsSettingsNode = settingsNode->FirstChildElement("graphics");

        std::string driverName = graphicsSettingsNode->FirstChildElement("driver")->GetText();

        int resolutionWidth = graphicsSettingsNode->FirstChildElement("resolution")->IntAttribute("width", 640);
        int resolutionHeight = graphicsSettingsNode->FirstChildElement("resolution")->IntAttribute("height", 480);
        int colorDepth = graphicsSettingsNode->FirstChildElement("colorDepth")->IntText(16);

        bool fullScreen = graphicsSettingsNode->FirstChildElement("fullScreen")->BoolText(false);
        bool vsync = graphicsSettingsNode->FirstChildElement("vsync")->BoolText(false);
        bool stencil = graphicsSettingsNode->FirstChildElement("stencilBuffer")->BoolText(false);

        return Settings{ driverName, resolutionWidth, resolutionHeight, colorDepth, fullScreen, vsync, stencil };
    }

    virtual std::vector<std::shared_ptr<Level>> loadLevels() {
        std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();

        tinyxml2::XMLError xmlError = xml->LoadFile("Data/levels.xml");

        if (xmlError != tinyxml2::XML_SUCCESS) {
            std::cerr << "Can not load levels.xml file" << std::endl;
            throw "Can not load levels";
        }

        auto levelsNode = xml->FirstChildElement("levels");

        auto levelNode = levelsNode->FirstChildElement("level");
        auto lastLevelNode = levelsNode->LastChildElement("level");

        std::vector<std::shared_ptr<Level>> levels;

        while (levelNode != nullptr) {
            std::string meshName = levelNode->FirstChildElement("model")->GetText();

            auto levelDescriptor = std::make_shared<Level>(meshName);

            auto targetsNode = levelNode->FirstChildElement("targets");

            auto targetNode = targetsNode->FirstChildElement("target");
            auto lastTargetNode = targetsNode->LastChildElement("target");

            while (targetNode != nullptr) {
                auto positionNode = targetNode->FirstChildElement("position");

                irr::core::vector3df position = irr::core::vector3df(positionNode->FloatAttribute("x", 0.0f), positionNode->FloatAttribute("y", 0.0f), positionNode->FloatAttribute("z", 0.0f));

                levelDescriptor->addTargetPosition(position);

                targetNode = targetNode->NextSiblingElement("target");
            }

            levels.push_back(levelDescriptor);

            levelNode = levelNode->NextSiblingElement("level");
        }

        return levels;
    }
};

#define MAX_TIME 6000

class IrrlichtRenderer : public Renderer {
public:
    IrrlichtRenderer(std::shared_ptr<GameState> _gameState) : Renderer(std::move(_gameState)) {}

    virtual void init(Settings settings) {
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

    virtual void processActionQueue() {
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

    void processAction(PlaySoundAction* action) {
        soundEngine->play2D(action->getSoundFile().c_str(), false);
    }

    void processAction(LoadFirstLevelAction* action) {
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

    void processAction(LoadNextLevelAction* action) {
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

    void processAction(TargetEliminatedAction* action) {
        action->getTarget()->setVisible(false);
        gameState->getCurrentScore()->targetEliminated();
        
        if (gameState->getCurrentScore()->getTargetsEliminated() >= gameState->getCurrentLevel()->getTargets().size()) {
            // TODO: show next level menu
            gameState->enqueue(new LoadNextLevelAction(gameState->getCurrentLevel(), gameState->getNextLevel()));
        }
    }

    virtual void render() {
        // TODO: here should the menu logic be
        driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

        smgr->drawAll();
        guienv->drawAll();

        updateStatusBar();

        driver->endScene();
    }

    virtual void shutdown() {
        device->drop();
        soundEngine->drop();

        timer->stop();
    }

    virtual bool isRunning() {
        // TODO: add (main menu) state checks
        return device->run();
    }

protected:
    // TODO: this is the endgame screen
    void showResult() {
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

    void updateStatusBar() {
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

private:
    std::shared_ptr<irr::IEventReceiver> eventReceiver;
    std::shared_ptr<InputHandler> inputHandler;

    // TODO: load from level?
    int Tm = MAX_TIME;

    irr::gui::IGUIStaticText* statusBar = 0;
    irr::ITimer* timer = 0;
    irr::gui::IGUIWindow* msgbox = 0;
    irr::gui::IGUIListBox* hiscoreTable = 0;

    std::shared_ptr<irr::IrrlichtDevice> device;
    std::shared_ptr<irr::video::IVideoDriver> driver;
    std::shared_ptr<irr::scene::ISceneManager> smgr;
    std::shared_ptr<irr::gui::IGUIEnvironment> guienv;

    std::shared_ptr<irr::scene::ICameraSceneNode> camera;

    irr::scene::IAnimatedMesh* playermesh = 0;
    irr::scene::IAnimatedMeshSceneNode* player = 0;

    irr::scene::IBillboardSceneNode* bill = 0;
    std::shared_ptr<irr::scene::ITriangleSelector> selector;

    std::shared_ptr<irrklang::ISoundEngine> soundEngine;
};

class Application {
public:
    Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<GameState> _gameState, std::shared_ptr<ResourceManager> _resourceManager) :
        renderer(std::move(_renderer)),
        gameState(std::move(_gameState)),
        inputHandler(std::make_shared<InputHandler>(gameState)),
        resourceManager(std::move(_resourceManager))
    {}

    void run() {
        gameState->setLevels(resourceManager->loadLevels());

        renderer->init(resourceManager->loadSettings());

        while (renderer->isRunning()) {
            renderer->processActionQueue();
            renderer->render();
        }

        renderer->shutdown();
    }

private:
    std::shared_ptr<GameState> gameState;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<InputHandler> inputHandler;
    std::shared_ptr<ResourceManager> resourceManager;
};

int main() {
    std::shared_ptr<ResourceManager> resourceManager = std::make_shared<ModernResourceManager>();
    std::shared_ptr<GameState> gameState = std::make_shared<GameState>();
    std::shared_ptr<Renderer> renderer = std::make_shared<IrrlichtRenderer>(gameState);

    std::shared_ptr<Application> application = std::make_shared<Application>(renderer, gameState, resourceManager);

    application->run();

    return 0;
}
