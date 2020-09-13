#pragma once

#include "Level.h"
#include "PlayerState.h"
#include "Score.h"

#include <string>
#include <memory>
#include <vector>
#include <queue>

enum class E_GAME_STATE {
    MAIN_MENU,
    PLAYING,
    END_LEVEL,
    END_GAME
};

/*
* Polymorphic method won't work:
*
* virtual void act(std::shared_ptr<Renderer> renderer) = 0
*
* You are fine with only Renderer's public or friend API.
* But if you want to use anything outside of Renderer otherwise available inside Renderer itself (like for loading resources) - you won't be able to.
* Also, I want to keep all the engine-specific logic in one place.
*/
enum class QueueActionType {
    PLAY_SOUND,
    LOAD_FIRST_LEVEL,
    LOAD_NEXT_LEVEL,
    TARGET_ELIMINATED,
};

class QueueAction {
protected:
    QueueAction(const QueueActionType _type);

public:
    const QueueActionType getType() const;

private:
    QueueActionType type;
};

class PlaySoundAction : public QueueAction {
public:
    PlaySoundAction(std::string _soundFile);

    const std::string getSoundFile() const;

private:
    std::string soundFile;
};

class LoadNextLevelAction : public QueueAction {
public:
    LoadNextLevelAction(std::shared_ptr<Level> _previousLevel, std::shared_ptr<Level> _nextLevel);

    const std::shared_ptr<Level> getPreviousLevel() const;

    const std::shared_ptr<Level> getNextLevel() const;

private:
    std::shared_ptr<Level> previousLevel;
    std::shared_ptr<Level> nextLevel;
};

class LoadFirstLevelAction : public QueueAction {
public:
    LoadFirstLevelAction(std::shared_ptr<Level> _level);

    const std::shared_ptr<Level> getLevel() const;

private:
    std::shared_ptr<Level> level;
};

class TargetEliminatedAction : public QueueAction {
public:
    TargetEliminatedAction(std::shared_ptr<irr::scene::ISceneNode> _target);

    const std::shared_ptr<irr::scene::ISceneNode> getTarget() const;

private:
    std::shared_ptr<irr::scene::ISceneNode> target;
};

class GameState {
public:
    GameState();

    void timeElapsed(unsigned long time);

    const std::shared_ptr<Score> getCurrentScore() const;

    const E_GAME_STATE getCurrentState() const;

    const std::shared_ptr<Level> getCurrentLevel() const;

    const std::shared_ptr<PlayerState> getPlayerState() const;

    const int getCurrentLevelIndex() const;

    const int getLevelsCnt() const;

    void nextLevelLoaded();

    void enqueue(QueueAction* action);

    const bool hasActions() const;

    QueueAction* nextAction();

    void setLevels(std::vector<std::shared_ptr<Level>> _levels);


    const std::shared_ptr<Level> getNextLevel() const;

private:
    E_GAME_STATE currentState;

    std::shared_ptr<Score> currentScore;
    std::shared_ptr<PlayerState> playerState;

    std::queue<QueueAction*> actionQueue;

    std::vector<std::shared_ptr<Level>> levels;
    size_t currentLevel;
};
