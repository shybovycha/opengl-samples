#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include <irrlicht.h>

#include "GameState.h"
#include "ActionDispatcher.h"
#include "IrrlichtHUD.h"
#include "IrrlichtEventReceiver.h"
#include "Renderer.h"
#include "Settings.h"

class IrrlichtRenderer : public Renderer {
public:
    IrrlichtRenderer(std::shared_ptr<GameState> _gameState, std::shared_ptr<ActionDispatcher> _actionDispatcher);

    virtual void init(Settings settings);

    virtual void processActionQueue();

    void processAction(PlaySoundAction* action);

    void processAction(LoadFirstLevelAction* action);

    void processAction(LoadNextLevelAction* action);

    void processAction(TargetEliminatedAction* action);

    void processAction(StartNewGameAction* action);

    void processAction(MainMenuAction* action);

    void processAction(HideMainMenuAction* action);

    void processAction(QuitAction* action);

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

    void updateTimer();

private:
    std::shared_ptr<irr::IEventReceiver> eventReceiver;
    std::shared_ptr<ActionDispatcher> actionDispatcher;
    std::shared_ptr<IrrlichtHUD> hud;

    irr::ITimer* timer = 0;
    irr::gui::IGUIWindow* msgbox = 0;
    irr::gui::IGUIListBox* hiscoreTable = 0;
    irr::gui::IGUIWindow* mainMenuWindow = 0;

    irr::IrrlichtDevice* device = 0;
    irr::video::IVideoDriver* driver = 0;
    irr::scene::ISceneManager* smgr = 0;
    irr::gui::IGUIEnvironment* guienv = 0;

    irr::scene::ICameraSceneNode* camera = 0;

    irr::scene::IAnimatedMesh* playermesh = 0;
    irr::scene::IAnimatedMeshSceneNode* player = 0;

    irr::scene::IBillboardSceneNode* bill = 0;
    irr::scene::ITriangleSelector* selector = 0;

    // irrklang::ISoundEngine* soundEngine = 0;
};
