#include "QueueAction.h"

QueueAction::QueueAction(const QueueActionType _type) : type(_type)
{
}

const QueueActionType QueueAction::getType() const
{
    return type;
}

PlaySoundAction::PlaySoundAction(std::string _soundFile) : QueueAction(QueueActionType::PLAY_SOUND),
                                                           soundFile(_soundFile)
{
}

const std::string PlaySoundAction::getSoundFile() const
{
    return soundFile;
}

LoadNextLevelAction::LoadNextLevelAction(std::shared_ptr<Level> _previousLevel, std::shared_ptr<Level> _nextLevel)
        : QueueAction(QueueActionType::LOAD_NEXT_LEVEL), previousLevel(_previousLevel), nextLevel(_nextLevel)
{
}

const std::shared_ptr<Level> LoadNextLevelAction::getPreviousLevel() const
{
    return previousLevel;
}

const std::shared_ptr<Level> LoadNextLevelAction::getNextLevel() const
{
    return nextLevel;
}

LoadFirstLevelAction::LoadFirstLevelAction(std::shared_ptr<Level> _level) : QueueAction(
        QueueActionType::LOAD_FIRST_LEVEL), level(_level)
{
}

const std::shared_ptr<Level> LoadFirstLevelAction::getLevel() const
{
    return level;
}

TargetEliminatedAction::TargetEliminatedAction(SceneNode* _target) : QueueAction(
        QueueActionType::TARGET_ELIMINATED), target(_target)
{
}

SceneNode* TargetEliminatedAction::getTarget() const
{
    return target;
}

StartNewGameAction::StartNewGameAction() : QueueAction(QueueActionType::START_NEW_GAME)
{
}

QuitAction::QuitAction() : QueueAction(QueueActionType::QUIT)
{
}

MainMenuAction::MainMenuAction() : QueueAction(QueueActionType::MAIN_MENU)
{
}

HideMainMenuAction::HideMainMenuAction() : QueueAction(QueueActionType::HIDE_MAIN_MENU)
{
}

GameOverAction::GameOverAction() : QueueAction(QueueActionType::GAME_OVER)
{
}
