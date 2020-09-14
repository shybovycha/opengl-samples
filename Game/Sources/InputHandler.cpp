#include "InputHandler.h"

InputHandler::InputHandler(std::shared_ptr<GameState> _gameState) : gameState(std::move(_gameState)) {}

void InputHandler::shoot(irr::scene::ISceneNode* objectAtCursor) {
    if (gameState->getPlayerState()->getCurrentAmmo() <= 0) {
        gameState->enqueue(new PlaySoundAction("Resources/Sounds/noammo.wav"));
        return;
    }

    gameState->enqueue(new PlaySoundAction("Resources/Sounds/shot.wav"));

    gameState->getPlayerState()->shoot();

    if (objectAtCursor == gameState->getCurrentLevel()->getModel().get()) {
        return;
    }

    for (auto target : gameState->getCurrentLevel()->getTargets()) {
        if (target.get() != objectAtCursor) {
            continue;
        }

        gameState->enqueue(new TargetEliminatedAction(std::move(target)));
        gameState->enqueue(new PlaySoundAction("Resources/Sounds/bell.wav"));

        break;
    }
}

void InputHandler::reload() {
    gameState->getPlayerState()->reload();
    gameState->enqueue(new PlaySoundAction("Resources/Sounds/reload.wav"));
}

void InputHandler::mainMenu() {
    // TODO: implement properly
    exit(0);
}

void InputHandler::targetEliminated() {
    gameState->getCurrentScore()->targetEliminated();
}

void InputHandler::loadNextLevel() {
    gameState->enqueue(new LoadNextLevelAction(gameState->getCurrentLevel(), gameState->getNextLevel()));
}

void InputHandler::loadFirstLevel() {
    gameState->enqueue(new LoadFirstLevelAction(gameState->getCurrentLevel()));
}

void InputHandler::nextLevelLoaded() {
    gameState->nextLevelLoaded();
    gameState->getCurrentScore()->resetTargetEliminated();
}

void InputHandler::levelsLoaded(std::vector<std::shared_ptr<Level>> levels) {
    gameState->setLevels(levels);
}
