#pragma once

#include <memory>
#include <vector>

#include "GameState.h"
#include "Level.h"
#include "QueueAction.h"

class ActionDispatcher {
public:
    ActionDispatcher(std::shared_ptr<GameState> _gameState);

    void shoot(irr::scene::ISceneNode* objectAtCursor);

    void reload();

    void mainMenu();

    void hideMainMenu();

    void targetEliminated();

    void loadNextLevel();

    void loadFirstLevel();

    void nextLevelLoaded();

    void firstLevelLoaded();

    void levelsLoaded(std::vector<std::shared_ptr<Level>> levels);

    void startNewGame();

    void quit();

private:
    std::shared_ptr<GameState> gameState;
};
