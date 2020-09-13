#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>

#include <irrlicht.h>
#include <irrKlang.h>

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

class Level {
public:
    Level(const std::string filename) : filename(filename) {}

    void load() {
        // TODO: implement
    }

private:
    std::string filename;

    std::vector<irr::core::vector3df> targetPositions;

    std::shared_ptr<irr::scene::ISceneNode> model;
};

class Score {
public:
    Score() : targetsEliminated(0), currentTime(0) {}

    void targetEliminated() {
        targetsEliminated++;
    }

    void timeUsed(unsigned long time) {
        currentTime += time;
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
    PlayerState() : currentAmmo(0), maxAmmo(0) {}

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

class GameState {
public:
    GameState() : currentState(E_GAME_STATE::MAIN_MENU), currentScore(std::make_unique<Score>()), playerState(std::make_unique<PlayerState>()) {}

    void loadLevels(const std::string filename) {
        // TODO
    }

    void nextLevel() {
        // TODO
    }

    void targetEliminated() {
        currentScore->targetEliminated();
    }

    void timeElapsed(unsigned long time) {
        currentScore->timeUsed(time);
    }

    const std::unique_ptr<Score>& getCurrentScore() const {
        return std::move(currentScore);
    }

    const E_GAME_STATE getCurrentState() const {
        return currentState;
    }

    const std::shared_ptr<Level> getCurrentLevel() const {
        if (currentLevel < 0 || currentLevel >= levels.size()) {
            throw "Invalid current level index";
        }

        return std::move(levels.at(currentLevel));
    }

    const std::unique_ptr<PlayerState>& getPlayerState() const {
        return std::move(playerState);
    }

private:
    E_GAME_STATE currentState;

    std::unique_ptr<Score> currentScore;
    std::unique_ptr<PlayerState> playerState;

    std::vector<std::shared_ptr<Level>> levels;
    size_t currentLevel;
};

class InputHandler {
public:
    InputHandler(std::unique_ptr<GameState> _gameState) : gameState(std::move(_gameState)) {}

    void shootAction() {
        // TODO: implement
    }

    void reloadAction() {
        // TODO: implement
    }

private:
    std::unique_ptr<GameState> gameState;
};

class ModernEventReceiver : public irr::IEventReceiver {
public:
    ModernEventReceiver(std::unique_ptr<InputHandler> _inputHandler) : inputHandler(std::move(_inputHandler)) {}

    virtual bool OnEvent(const irr::SEvent& event) {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
                handleMouseClickLeft();
            }
            else if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
                handleMouseClickRight();
            }
        }
    }

protected:
    void handleMouseClickLeft() {
        // TODO: implement
    }

    void handleMouseClickRight() {
        // TODO: implement
    }

private:
    std::unique_ptr<InputHandler> inputHandler;
};

class Renderer {
public:
    Renderer(std::unique_ptr<GameState> _gameState) : gameState(std::move(_gameState)) {}

    virtual void init() = 0;

    virtual void render() = 0;

    virtual void shutdown() = 0;

    virtual bool isRunning() = 0;

private:
    std::unique_ptr<GameState> gameState;
};

class IrrlichtRenderer : public Renderer {
public:
    IrrlichtRenderer(std::unique_ptr<GameState> _gameState) : Renderer(std::move(_gameState)) {}

    virtual void init() {
        // TODO: implement
    }

    virtual void render() {
        // TODO: implement
    }

    virtual void shutdown() {
        // TODO: implement
    }

    virtual bool isRunning() {
        // TODO: implement
        return true;
    }
};

class Application {
public:
    Application() {}

    void run() {
        renderer->init();

        while (renderer->isRunning()) {
            renderer->render();
        }

        renderer->shutdown();
    }

private:
    std::unique_ptr<GameState> gameState;
    std::unique_ptr<Renderer> renderer;
};

/*
    --------------------------------------------

    HERE BE DRAGONS!

    The code below was written in 2007 (approx.), with very few modifications made
    so it compiles and runs.

    Be patient until the whole thing is reworked.

    --------------------------------------------
*/

#define MAX_AMMO 10
#define MAX_TIME 6000

int targetCnt = 0, Tm = MAX_TIME, levelNumber = 0;
int points = 0, targetLeft = 0, ammo = MAX_AMMO;
int Tms = 0, Pnts = 0;

bool endLevel = false;
bool hiscoremnu = false;

char* maps[10];
irr::core::vector3df positions[100];
int targets[100] = { -1 }, levelCnt, shoots = 0;

irrklang::ISound* music = 0;

struct TRecord
{
    char name[128];
    int time;
    int points;
};

int hiscoreCnt = -1;
TRecord hiscores[100];

irr::gui::IGUIStaticText* indicator = 0;
irr::ITimer* timer = 0;
irr::gui::IGUIWindow* msgbox = 0;
irr::gui::IGUIListBox* hiscoreTable = 0;

irr::IrrlichtDevice* device = 0;
irr::video::IVideoDriver* driver = 0;
irr::scene::ISceneManager* smgr = 0;
irr::gui::IGUIEnvironment* guienv = 0;
irr::scene::IAnimatedMesh* levelmesh = 0;
irr::scene::IAnimatedMeshSceneNode* level = 0;
irr::scene::ICameraSceneNode* camera = 0;

irr::scene::IAnimatedMesh* playermesh = 0;
irr::scene::IAnimatedMeshSceneNode* player = 0;

irr::scene::IBillboardSceneNode* bill = 0;
irr::scene::ITriangleSelector* selector = 0;

irrklang::ISoundEngine* engine = 0;

irr::scene::ISceneNode* target[10] = { 0 };

irr::scene::ITriangleSelector* trisel = 0;

void gotoMap(int mapNum);
void showHiscores();
void saveHiscores();

class EventReceiver : public irr::IEventReceiver
{
public:
    virtual bool OnEvent(const irr::SEvent& event)
    {
        if (event.EventType == irr::EET_GUI_EVENT)
        {
            if (event.GUIEvent.EventType == irr::gui::EGET_MESSAGEBOX_OK)
            {
                if (levelNumber + 1 == levelCnt)
                {
                    saveHiscores();
                    device->drop();
                    engine->drop();

                    exit(0);
                }

                if (endLevel == true)
                {
                    //showHiscores();

                    endLevel = false;
                    gotoMap(++levelNumber);
                    Tm = MAX_TIME;

                    return true;
                }
            }
        }

        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
        {
            if (event.KeyInput.Key == irr::KEY_ESCAPE)
            {
                saveHiscores();
                device->drop();
                engine->drop();

                exit(0);
            }

            if (event.KeyInput.Key == irr::KEY_RETURN)
            {
                if (hiscoremnu == true)
                {
                    hiscoremnu = false;
                }
            }
        }

        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            irr::scene::ISceneNode* object = 0;

            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
            {
                object = smgr->getSceneCollisionManager()->getSceneNodeFromCameraBB(camera);

                if (ammo <= 0)
                {
                    engine->play2D("Resources/Sounds/noammo.wav", false);
                    return false;
                }

                engine->play2D("Resources/Sounds/shot.wav", false);
                ammo--;
                shoots++;

                if (object == level || object == player)
                    return false;

                for (int i = 0; i <= 9; i++)
                    if (target[i] == object)
                    {
                        object->setVisible(false);

                        points++;
                        targetLeft--;

                        engine->play2D("Resources/Sounds/bell.wav", false);

                        return true;
                    }
            }

            if (event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
            {
                if (ammo < MAX_AMMO)
                {
                    ammo = MAX_AMMO;
                    engine->play2D("Resources/Sounds/reload.wav", false);

                    return true;
                }
            }
        }

        return false;
    }
};

EventReceiver receiver;

void init()
{
    device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, false);

    device->setWindowCaption(L"ShootThem!");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    device->setEventReceiver(&receiver);

    engine = irrklang::createIrrKlangDevice();

    device->getFileSystem()->addZipFileArchive("Resources/Packs/data.pk3");

    irr::scene::IAnimatedMesh* mesh = 0;
    mesh = smgr->getMesh("chicken.3ds");

    for (int i = 0; i <= 9; i++)
    {
        target[i] = smgr->addAnimatedMeshSceneNode(mesh);
        target[i]->setVisible(false);

        target[i]->setMaterialTexture(0, driver->getTexture("Chick02.bmp"));
        target[i]->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);
    }

    bill = smgr->addBillboardSceneNode();
    bill->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
    bill->setMaterialTexture(0, driver->getTexture("cross.bmp"));
    bill->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    bill->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    bill->setSize(irr::core::dimension2d<irr::f32>(20.0f, 20.0f));

    smgr->addLightSceneNode(0, irr::core::vector3df(0, 20, 0), irr::video::SColorf(0.5f, 0.5f, 0.5f, 0.5f), 3000, 0);

    driver->setFog(irr::video::SColor(0, 138, 125, 81), irr::video::EFT_FOG_LINEAR, 250, 1000, 0, true);
}

void createPlayer()
{
    camera = smgr->addCameraSceneNodeFPS(0, 100, 0, 0);
    device->getCursorControl()->setVisible(false);

    indicator = guienv->addStaticText(L"New game", irr::core::rect<irr::s32>(10, 10, 260, 22), true, true, 0, 0, true);
    timer = device->getTimer();
    timer->start();

    playermesh = smgr->getMesh("rifle.3ds");
    player = smgr->addAnimatedMeshSceneNode(playermesh);

    player->setPosition(irr::core::vector3df(0.5f, -1.0f, 1.0f));

    player->setParent(camera);
}

void loadCoords(char* filename)
{
    std::ifstream inf(filename);

    int i = 0, oldI = 0;

    inf >> levelCnt;

    for (int t = 0; t <= levelCnt - 1; t++)
    {
        inf >> targets[t];

        for (i = oldI; i <= oldI + targets[t] - 1; i++)
            inf >> positions[i].X >> positions[i].Y >> positions[i].Z;

        oldI = i;
    }

    inf.close();
}

void loadHiscores(char* filename)
{
    std::ifstream inf(filename);

    if (!inf.is_open()) {
        return;
    }

    inf >> hiscoreCnt;

    for (int i = 0; i <= hiscoreCnt - 1; i++)
    {
        inf >> hiscores[i].name;
        inf >> hiscores[i].time;
        inf >> hiscores[i].points;
    }

    inf.close();
}

void saveHiscores()
{
    std::ofstream outf("Data/hiscores.dat");

    outf << ++hiscoreCnt;

    //hiscores[hiscoreCnt].name = "Player";
    hiscores[hiscoreCnt].time = Tms;
    hiscores[hiscoreCnt].points = Pnts;

    for (int i = 0; i <= hiscoreCnt - 1; i++)
    {
        outf << hiscores[i].name << std::endl;
        outf << hiscores[i].time << std::endl;
        outf << hiscores[i].points << std::endl;
    }

    outf.close();
}

void createConfig()
{
    maps[0] = "room1.x";
    maps[1] = "egypt1.x";
    maps[2] = "forest1.x";
    maps[3] = "square1.x";
}

void loadMap(char* mapname)
{
    if (level)
        level->setVisible(false);

    levelmesh = smgr->getMesh(mapname);
    level = smgr->addAnimatedMeshSceneNode(levelmesh);

    selector = smgr->createOctTreeTriangleSelector(levelmesh->getMesh(0), level, 128);
}

void pasteTargets(int levelNumber)
{
    for (int i = 0; i <= targets[levelNumber] - 1; i++)
    {
        target[i]->setVisible(true);

        int k = targets[levelNumber - 1];

        target[i]->setPosition(positions[k + i]);
    }

    targetLeft = targets[levelNumber];
    targetCnt = targetLeft;
    points = 0;
    shoots = 0;
}

void gotoMap(int mapNum)
{
    loadMap(maps[levelNumber]);
    pasteTargets(mapNum);

    timer->start();
}

void showResult()
{
    irr::core::stringw title = L"Level complete!";
    
    std::wostringstream msg;
    msg << "Your time: " << (MAX_TIME/ 100) - abs(Tm / 100) << "sec;  Shots: " << shoots << "/" << targets[levelNumber] << " min" << ";  Target hit: " << points << "/" << targets[levelNumber];

    Tms += (MAX_TIME / 100) - abs(Tm / 100);
    Pnts += points;

    guienv->addMessageBox(title.c_str(), msg.str().(), true, irr::gui::EMBF_OK, 0, 0);

    endLevel = true;

    timer->stop();

    if (levelNumber + 1 == levelCnt)
    {
        guienv->getSkin()->setFont(guienv->getFont("fontcourier.bmp"));
        guienv->clear();
        guienv->addMessageBox(L"Congratulations!", L"Game over!", true, irr::gui::EMBF_OK, 0, 0);

        while (device->run())
        {
            driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

            smgr->drawAll();
            guienv->drawAll();

            driver->endScene();
        }
    }
}

void showHiscores()
{
    hiscoremnu = true;

    hiscoreTable = guienv->addListBox(irr::core::rect<irr::s32>(10, 30, 300, 100), 0, 0, true);

    for (int i = 0; i <= hiscoreCnt - 1; i++)
    {
        std::wostringstream str;

        str << (i + 1) << " - " << hiscores[i].name << " - " << hiscores[i].time << " sec. - " << hiscores[i].points << " pts.";

        hiscoreTable->addItem(str.str().c_str());
    }

    timer->stop();

    while (hiscoremnu == true && device->run())
    {
        driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

        smgr->drawAll();
        guienv->drawAll();

        driver->endScene();
    }

    timer->start();
}

void refreshIndicator()
{
    std::wostringstream statusString;
    statusString << "Ammo: " << ammo << "/" << MAX_AMMO << "; Points: " << points << "/" << targetCnt << "; Time:" << Tm / 100 << "; Level:" << levelNumber + 1 << "/" << levelCnt;

    if (!timer->isStopped() && endLevel == false)
    {
        Tm--;
    }

    if (Tm <= 0 || points == targetCnt)
        if (endLevel == false)
            showResult();

    indicator->setText(statusString.str().c_str());

    float k = (sin(abs(Tm) / 100) / (10 - levelNumber));

    camera->setRotation(irr::core::vector3df(camera->getRotation().X + k,
        camera->getRotation().Y, camera->getRotation().Z));

    irr::core::line3d<irr::f32> line;
    line.start = camera->getPosition();
    line.end = line.start + (camera->getTarget() - line.start).normalize() * 10000.0f;

    irr::core::vector3df intersection;
    irr::core::triangle3df tri;

    irr::scene::ISceneNode* node = 0;

    if (smgr->getSceneCollisionManager()->getCollisionPoint(line, selector, intersection, tri, node))
        bill->setPosition(intersection);
}

int main()
{
    init();

    createPlayer();
    createConfig();
    loadCoords("Data/coords.dat");
    loadHiscores("Data/hiscores.dat");

    gotoMap(0);

    while (device->run())
    {
        driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

        smgr->drawAll();
        guienv->drawAll();

        refreshIndicator();

        driver->endScene();
    }

    saveHiscores();

    device->drop();
    engine->drop();

    timer->stop();

    return 0;
}