#include "InputHandler.h"

InputHandler::InputHandler(std::shared_ptr<GameState> _gameState) : gameState(std::move(_gameState)) {}

void InputHandler::shootAction(irr::scene::ISceneNode* objectAtCursor) {
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

void InputHandler::reloadAction() {
    gameState->getPlayerState()->reload();
    gameState->enqueue(new PlaySoundAction("Resources/Sounds/reload.wav"));
}

void InputHandler::mainMenuAction() {
    // TODO: implement properly
    exit(0);
}
