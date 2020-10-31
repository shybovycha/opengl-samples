#pragma once

#include <memory>
#include <string>

#include "../game/Level.h"

enum class GameStateType
{
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
enum class QueueActionType
{
    HIDE_MAIN_MENU,
    LOAD_FIRST_LEVEL,
    LOAD_NEXT_LEVEL,
    MAIN_MENU,
    PLAY_SOUND,
    QUIT,
    START_NEW_GAME,
    TARGET_ELIMINATED,
    GAME_OVER
};

class QueueAction
{
protected:
    QueueAction(const QueueActionType _type);

public:
    const QueueActionType getType() const;

private:
    QueueActionType type;
};

class PlaySoundAction : public QueueAction
{
public:
    PlaySoundAction(std::string _soundFile);

    const std::string getSoundFile() const;

private:
    std::string soundFile;
};

class LoadNextLevelAction : public QueueAction
{
public:
    LoadNextLevelAction(std::shared_ptr<Level> _previousLevel, std::shared_ptr<Level> _nextLevel);

    const std::shared_ptr<Level> getPreviousLevel() const;

    const std::shared_ptr<Level> getNextLevel() const;

private:
    std::shared_ptr<Level> previousLevel;
    std::shared_ptr<Level> nextLevel;
};

class LoadFirstLevelAction : public QueueAction
{
public:
    LoadFirstLevelAction(std::shared_ptr<Level> _level);

    const std::shared_ptr<Level> getLevel() const;

private:
    std::shared_ptr<Level> level;
};

class TargetEliminatedAction : public QueueAction
{
public:
    TargetEliminatedAction(SceneNode* _target);

    SceneNode* getTarget() const;

private:
    SceneNode* target;
};

class StartNewGameAction : public QueueAction
{
public:
    StartNewGameAction();
};

class QuitAction : public QueueAction
{
public:
    QuitAction();
};

class MainMenuAction : public QueueAction
{
public:
    MainMenuAction();
};

class HideMainMenuAction : public QueueAction
{
public:
    HideMainMenuAction();
};

class GameOverAction : public QueueAction
{
public:
    GameOverAction();
};
