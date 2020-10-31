#pragma once

#include <memory>
#include <vector>

#include "../game/GameState.h"
#include "../game/Level.h"
#include "QueueAction.h"

class ActionDispatcher
{
public:
    ActionDispatcher(std::shared_ptr<GameState> _gameState);

    void shoot(SceneNode* objectAtCursor);

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

    void gameOver();

    void quit();

private:
    std::shared_ptr<GameState> gameState;
};
