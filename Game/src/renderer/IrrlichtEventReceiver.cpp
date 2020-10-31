#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<GameState> _gameState,
        std::shared_ptr<ActionDispatcher> _actionDispatcher, irr::scene::ISceneManager* _sceneManager,
        irr::scene::ICameraSceneNode* _camera) :
        actionDispatcher(_actionDispatcher),
        gameState(_gameState),
        sceneManager(_sceneManager),
        camera(_camera)
{
}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event)
{
    if (gameState->getCurrentState() == GameStateType::PLAYING)
    {
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
            {
                const irr::f32 MAX_RAYCAST_DISTANCE = 10000.0f;
                const irr::f32 RAY_OFFSET_DISTANCE = 10.0f;

                irr::core::vector3df cameraDirection = camera->getTarget() - camera->getAbsolutePosition();
                
                irr::core::line3df ray(
                    camera->getAbsolutePosition() + (cameraDirection * RAY_OFFSET_DISTANCE),
                    cameraDirection * MAX_RAYCAST_DISTANCE
                );

                irr::scene::ISceneNode* objectAtCursor = sceneManager->getSceneCollisionManager()
                    ->getSceneNodeFromRayBB(ray);

                actionDispatcher->shoot(new IrrlichtSceneNode(objectAtCursor));
            }
            else if (event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
            {
                actionDispatcher->reload();
            }
        }
    }
    
    if (gameState->getCurrentState() == GameStateType::PLAYING || gameState->getCurrentState() == GameStateType::END_GAME)
    {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
        {
            if (event.KeyInput.Key == irr::KEY_ESCAPE)
            {
                actionDispatcher->mainMenu();
            }
        }
    }
    
    {
        if (event.EventType == irr::EET_GUI_EVENT)
        {
            if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED)
            {
                if (event.GUIEvent.Caller->getID() == QUIT_BUTTON_ID)
                {
                    actionDispatcher->quit();
                }
                else if (event.GUIEvent.Caller->getID() == NEW_GAME_BUTTON_ID)
                {
                    actionDispatcher->startNewGame();
                }
                else if (event.GUIEvent.Caller->getID() == CONTINUE_BUTTON_ID)
                {
                    actionDispatcher->hideMainMenu();
                }
            }
        }
    }

    return false;
}
