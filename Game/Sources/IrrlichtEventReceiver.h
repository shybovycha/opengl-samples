#pragma once

#include "GameState.h"
#include "InputHandler.h"

#include <memory>

#include <irrlicht.h>

class IrrlichtEventReceiver : public irr::IEventReceiver {
public:
    IrrlichtEventReceiver(std::shared_ptr<InputHandler> _inputHandler, std::shared_ptr<irr::scene::ISceneManager> _sceneManager, std::shared_ptr<irr::scene::ICameraSceneNode> _camera);

    virtual bool OnEvent(const irr::SEvent& event);

private:
    std::shared_ptr<InputHandler> inputHandler;
    std::shared_ptr<irr::scene::ISceneManager> sceneManager;
    std::shared_ptr<irr::scene::ICameraSceneNode> camera;
};
