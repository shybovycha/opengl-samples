#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include <irrlicht.h>
#include <irrKlang.h>

#include "../core/ActionDispatcher.h"
#include "../core/Renderer.h"
#include "../core/SceneNode.h"
#include "../core/Settings.h"
#include "../game/GameState.h"
#include "CDrunkShaderCallback.h"
#include "CScreenQuadSceneNode.h"
#include "IrrlichtEventReceiver.h"
#include "IrrlichtHUD.h"
#include "IrrlichtSceneNode.h"

class IrrlichtRenderer : public Renderer
{
public:
    IrrlichtRenderer(std::shared_ptr<GameState> _gameState, std::shared_ptr<ActionDispatcher> _actionDispatcher);

    virtual void init(std::shared_ptr<Settings> settings);

    virtual void processActionQueue();

    void processAction(PlaySoundAction* action);

    void processAction(LoadFirstLevelAction* action);

    void processAction(LoadNextLevelAction* action);

    void processAction(TargetEliminatedAction* action);

    void processAction(StartNewGameAction* action);

    void processAction(MainMenuAction* action);

    void processAction(HideMainMenuAction* action);

    void processAction(QuitAction* action);

    void processAction(GameOverAction* action);

    virtual void render();

    virtual void shutdown();

    virtual bool isRunning();

    void updateCrosshair();

    void updatePostProcessingEffects();

    void renderMainMenu();

    void renderEndGameMenu();

    void renderEndLevelMenu();

protected:
    // TODO: this is the endgame screen
    void showResult();

    void loadLevel(std::shared_ptr<Level> levelDescriptor);

    void unloadLevel(std::shared_ptr<Level> levelDescriptor);

    void updateTimer();

private:
    std::shared_ptr<irr::IEventReceiver> eventReceiver;
    std::shared_ptr<ActionDispatcher> actionDispatcher;
    std::shared_ptr<IrrlichtHUD> hud;

    irr::ITimer* timer = 0;
    irr::gui::IGUIWindow* msgbox = 0;
    irr::gui::IGUIListBox* hiscoreTable = 0;
    irr::gui::IGUIWindow* mainMenuWindow = 0;
    irr::gui::IGUIStaticText* gameOverLabel = 0;

    irr::IrrlichtDevice* device = 0;
    irr::video::IVideoDriver* driver = 0;
    irr::scene::ISceneManager* smgr = 0;
    irr::gui::IGUIEnvironment* guienv = 0;

    irr::scene::ICameraSceneNode* camera = 0;

    irr::scene::IAnimatedMesh* playermesh = 0;
    irr::scene::IAnimatedMeshSceneNode* player = 0;

    irr::scene::IBillboardSceneNode* bill = 0;
    irr::scene::ITriangleSelector* selector = 0;

    irrklang::ISoundEngine* soundEngine = 0;

    irr::video::ITexture* screenRenderTarget = 0;
    CScreenQuadSceneNode* screenQuad = 0;
    CDrunkShaderCallback* drunkShaderCallback = 0;
};
