#include "GameState.h"

GameState::GameState() : currentState(GameStateType::MAIN_MENU), currentScore(std::make_shared<Score>()), playerState(std::make_shared<PlayerState>()), currentLevel(0) {}

void GameState::timeElapsed(unsigned long time) {
    currentScore->timeUsed(time);
}

const std::shared_ptr<Score> GameState::getCurrentScore() const {
    return currentScore;
}

const GameStateType GameState::getCurrentState() const {
    return currentState;
}

const std::shared_ptr<Level> GameState::getCurrentLevel() const {
    if (levels.empty() || currentLevel < 0 || currentLevel >= levels.size()) {
        throw "Invalid current level index";
        return nullptr;
    }

    return levels.at(currentLevel);
}

const std::shared_ptr<PlayerState> GameState::getPlayerState() const {
    return playerState;
}

const int GameState::getCurrentLevelIndex() const {
    return currentLevel;
}

const int GameState::getLevelsCnt() const {
    return levels.size();
}

void GameState::nextLevelLoaded() {
    ++currentLevel;
}

void GameState::enqueue(QueueAction* action) {
    actionQueue.push(action);
}

const bool GameState::hasActions() const {
    return actionQueue.size() > 0;
}

void GameState::setCurrentState(GameStateType _state) {
    currentState = _state;
}

QueueAction* GameState::nextAction() {
    if (actionQueue.size() < 1) {
        return nullptr;
    }

    auto action = actionQueue.front();
    actionQueue.pop();
    return action;
}

void GameState::setLevels(std::vector<std::shared_ptr<Level>> _levels) {
    levels = _levels;
}

const std::shared_ptr<Level> GameState::getNextLevel() const
{
    if (currentLevel + 1 >= levels.size()) {
        return nullptr;
    }

    return levels.at(currentLevel + 1);
}
