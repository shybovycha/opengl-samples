#pragma once

#include <memory>
#include <vector>

#include "GameState.h"
#include "Level.h"

class ActionDispatcher {
public:
    ActionDispatcher(std::shared_ptr<GameState> _gameState);

    void shoot(irr::scene::ISceneNode* objectAtCursor);

    void reload();

    void mainMenu();

    void targetEliminated();

    void loadNextLevel();

    void loadFirstLevel();

    void nextLevelLoaded();

    void levelsLoaded(std::vector<std::shared_ptr<Level>> levels);

private:
    std::shared_ptr<GameState> gameState;
};
