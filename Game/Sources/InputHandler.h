#pragma once

#include "GameState.h"

#include <memory>

class InputHandler {
public:
    InputHandler(std::shared_ptr<GameState> _gameState);

    void shootAction(irr::scene::ISceneNode* objectAtCursor);

    void reloadAction();

    void mainMenuAction();

private:
    std::shared_ptr<GameState> gameState;
};
