#pragma once

#include <memory>

#include <irrlicht.h>

#include "../core/ActionDispatcher.h"
#include "../game/GameState.h"
#include "IrrlichtSceneNode.h"

#define NEW_GAME_BUTTON_ID 1
#define CONTINUE_BUTTON_ID 2
#define QUIT_BUTTON_ID 3

class IrrlichtEventReceiver : public irr::IEventReceiver
{
public:
    IrrlichtEventReceiver(std::shared_ptr<GameState> _gameState, std::shared_ptr<ActionDispatcher> _actionDispatcher,
            irr::scene::ISceneManager* _sceneManager, irr::scene::ICameraSceneNode* _camera);

    virtual bool OnEvent(const irr::SEvent& event);

private:
    std::shared_ptr<GameState> gameState;
    std::shared_ptr<ActionDispatcher> actionDispatcher;

    irr::scene::ISceneManager* sceneManager;
    irr::scene::ICameraSceneNode* camera;
};
