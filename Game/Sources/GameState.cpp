#include "GameState.h"

QueueAction::QueueAction(const QueueActionType _type) : type(_type) {}

const QueueActionType QueueAction::getType() const {
    return type;
}

PlaySoundAction::PlaySoundAction(std::string _soundFile) : QueueAction(QueueActionType::PLAY_SOUND), soundFile(_soundFile) {}

const std::string PlaySoundAction::getSoundFile() const {
    return soundFile;
}

LoadNextLevelAction::LoadNextLevelAction(std::shared_ptr<Level> _previousLevel, std::shared_ptr<Level> _nextLevel) : QueueAction(QueueActionType::LOAD_NEXT_LEVEL), previousLevel(_previousLevel), nextLevel(_nextLevel) {}

const std::shared_ptr<Level> LoadNextLevelAction::getPreviousLevel() const {
    return previousLevel;
}

const std::shared_ptr<Level> LoadNextLevelAction::getNextLevel() const {
    return nextLevel;
}

LoadFirstLevelAction::LoadFirstLevelAction(std::shared_ptr<Level> _level) : QueueAction(QueueActionType::LOAD_FIRST_LEVEL), level(_level) {}

const std::shared_ptr<Level> LoadFirstLevelAction::getLevel() const {
    return level;
}

TargetEliminatedAction::TargetEliminatedAction(std::shared_ptr<irr::scene::ISceneNode> _target) : QueueAction(QueueActionType::TARGET_ELIMINATED), target(_target) {}

const std::shared_ptr<irr::scene::ISceneNode> TargetEliminatedAction::getTarget() const {
    return target;
}

GameState::GameState() : currentState(E_GAME_STATE::MAIN_MENU), currentScore(std::make_shared<Score>()), playerState(std::make_shared<PlayerState>()), currentLevel(0) {}

void GameState::timeElapsed(unsigned long time) {
    currentScore->timeUsed(time);
}

const std::shared_ptr<Score> GameState::getCurrentScore() const {
    return currentScore;
}

const E_GAME_STATE GameState::getCurrentState() const {
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
