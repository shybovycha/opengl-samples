#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<ActionDispatcher> _actionDispatcher, std::shared_ptr<irr::scene::ISceneManager> _sceneManager, std::shared_ptr<irr::scene::ICameraSceneNode> _camera) :
    actionDispatcher(std::move(_actionDispatcher)),
    sceneManager(std::move(_sceneManager)),
    camera(std::move(_camera))
{}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event) {
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
    else if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.Key == irr::KEY_ESCAPE) {
            actionDispatcher->mainMenu();
        }
    }

    /*if (event.EventType == irr::EET_GUI_EVENT) {
        if (event.GUIEvent.EventType == irr::gui::EGET_MESSAGEBOX_OK) {
            if (levelNumber + 1 == levelCnt) {
                saveHiscores();
                device->drop();
                engine->drop();

                exit(0);
            }

            if (endLevel == true) {
                //showHiscores();

                endLevel = false;
                gotoMap(++levelNumber);
                Tm = MAX_TIME;

                return true;
            }
        }
    }

    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.Key == irr::KEY_ESCAPE) {
            saveHiscores();
            device->drop();
            engine->drop();

            exit(0);
        }

        if (event.KeyInput.Key == irr::KEY_RETURN) {
            if (hiscoremnu == true) {
                hiscoremnu = false;
            }
        }
    }*/

    return false;
}
