#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<GameState> _gameState, std::shared_ptr<ActionDispatcher> _actionDispatcher, irr::scene::ISceneManager* _sceneManager, irr::scene::ICameraSceneNode* _camera) :
    actionDispatcher(std::move(_actionDispatcher)),
    gameState(std::move(_gameState)),
    sceneManager(_sceneManager),
    camera(_camera)
{}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event) {
    if (gameState->getCurrentState() == GameStateType::PLAYING) {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
                irr::core::line3df ray(camera->getAbsolutePosition(), (camera->getTarget() - camera->getAbsolutePosition()) * 10000.0f);
                irr::scene::ISceneNode* objectAtCursor = sceneManager->getSceneCollisionManager()->getSceneNodeFromRayBB(ray); // getSceneNodeFromCameraBB(camera.get());

                actionDispatcher->shoot(objectAtCursor);
            }
            else if (event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN) {
                actionDispatcher->reload();
            }
        }

        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (event.KeyInput.Key == irr::KEY_ESCAPE) {
                actionDispatcher->mainMenu();
            }
        }
    } else {
        if (event.EventType == irr::EET_GUI_EVENT) {
            if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
                if (event.GUIEvent.Caller->getID() == QUIT_BUTTON_ID) {
                    actionDispatcher->quit();
                }
                else if (event.GUIEvent.Caller->getID() == NEW_GAME_BUTTON_ID) {
                    actionDispatcher->startNewGame();
                }
                else if (event.GUIEvent.Caller->getID() == CONTINUE_BUTTON_ID) {
                    actionDispatcher->hideMainMenu();
                }
            }
        }
    }

    return false;
}
