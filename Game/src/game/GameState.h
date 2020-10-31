#pragma once

#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <iostream>

#include "../core/QueueAction.h"
#include "Level.h"
#include "PlayerState.h"
#include "Score.h"

class GameState
{
public:
    GameState();

    const std::shared_ptr<Score> getCurrentScore() const;

    const GameStateType getCurrentState() const;

    const std::shared_ptr<Level> getCurrentLevel() const;

    const std::shared_ptr<PlayerState> getPlayerState() const;

    const int getCurrentLevelIndex() const;

    const int getLevelsCnt() const;

    const bool hasActions() const;

    const bool isGameOver() const;

    const bool isGameStarted() const;

    QueueAction* nextAction();

    const std::shared_ptr<Level> getNextLevel() const;

private:
    friend class ActionDispatcher;

    void timeElapsed(unsigned long time);

    void setLevels(std::vector<std::shared_ptr<Level>> _levels);

    void nextLevelLoaded();

    void enqueue(QueueAction* action);

    void setCurrentState(GameStateType _state);

    void startGame();

    void endGame();

    GameStateType currentState;

    std::shared_ptr<Score> currentScore;
    std::shared_ptr<PlayerState> playerState;

    std::queue<QueueAction*> actionQueue;

    std::vector<std::shared_ptr<Level>> levels;
    size_t currentLevel;

    bool hasGameStarted = false;
    bool hasGameEnded = false;
};
