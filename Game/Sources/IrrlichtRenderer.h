#pragma once

#include "GameState.h"
#include "InputHandler.h"
#include "IrrlichtEventReceiver.h"
#include "Renderer.h"
#include "Settings.h"

#include <irrlicht.h>
#include <irrKlang.h>

#include <memory>
#include <sstream>
#include <vector>

#define MAX_TIME 6000

class IrrlichtRenderer : public Renderer {
public:
    IrrlichtRenderer(std::shared_ptr<GameState> _gameState);

    virtual void init(Settings settings);

    virtual void processActionQueue();

    void processAction(PlaySoundAction* action);

    void processAction(LoadFirstLevelAction* action);

    void processAction(LoadNextLevelAction* action);

    void processAction(TargetEliminatedAction* action);

    virtual void render();

    virtual void shutdown();

    virtual bool isRunning();

protected:
    // TODO: this is the endgame screen
    void showResult();

    void updateStatusBar();

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
