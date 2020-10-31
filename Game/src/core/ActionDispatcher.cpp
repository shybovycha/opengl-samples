#include "ActionDispatcher.h"

ActionDispatcher::ActionDispatcher(std::shared_ptr<GameState> _gameState) : gameState(_gameState)
{
}

void ActionDispatcher::shoot(SceneNode* objectAtCursor)
{
    if (gameState->getPlayerState()->getCurrentAmmo() <= 0)
    {
        gameState->enqueue(new PlaySoundAction("resources/sounds/noammo.wav"));
        return;
    }

    gameState->enqueue(new PlaySoundAction("resources/sounds/shot.wav"));

    gameState->getPlayerState()->shoot();

    if (objectAtCursor == gameState->getCurrentLevel()->getModel())
    {
        return;
    }

    for (auto target : gameState->getCurrentLevel()->getTargets())
    {
        if (*target != *objectAtCursor)
        {
            continue;
        }

        gameState->enqueue(new TargetEliminatedAction(target));
        gameState->enqueue(new PlaySoundAction("resources/sounds/bell.wav"));

        break;
    }
}

void ActionDispatcher::reload()
{
    gameState->getPlayerState()->reload();
    gameState->enqueue(new PlaySoundAction("resources/sounds/reload.wav"));
}

void ActionDispatcher::mainMenu()
{
    gameState->setCurrentState(GameStateType::MAIN_MENU);
    gameState->enqueue(new MainMenuAction());
}

void ActionDispatcher::hideMainMenu()
{
    gameState->setCurrentState(GameStateType::PLAYING);
    gameState->enqueue(new HideMainMenuAction());
}

void ActionDispatcher::targetEliminated()
{
    gameState->getCurrentScore()->targetEliminated();
}

void ActionDispatcher::loadNextLevel()
{
    gameState->enqueue(new LoadNextLevelAction(gameState->getCurrentLevel(), gameState->getNextLevel()));
}

void ActionDispatcher::loadFirstLevel()
{
    gameState->enqueue(new LoadFirstLevelAction(gameState->getCurrentLevel()));
}

void ActionDispatcher::firstLevelLoaded()
{
    gameState->setCurrentState(GameStateType::PLAYING);
}

void ActionDispatcher::nextLevelLoaded()
{
    gameState->nextLevelLoaded();
    gameState->getCurrentScore()->resetTargetEliminated();
}

void ActionDispatcher::levelsLoaded(std::vector<std::shared_ptr<Level>> levels)
{
    gameState->setLevels(levels);
}

void ActionDispatcher::startNewGame()
{
    gameState->startGame();
    gameState->enqueue(new StartNewGameAction());
}

void ActionDispatcher::quit()
{
    gameState->enqueue(new QuitAction());
}

void ActionDispatcher::gameOver()
{
    gameState->setCurrentState(GameStateType::END_GAME);
    gameState->enqueue(new GameOverAction());
    gameState->endGame();
}
