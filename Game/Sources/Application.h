#pragma once

#include <memory>

#include "GameState.h"
#include "InputHandler.h"
#include "ResourceManager.h"
#include "Renderer.h"

class Application {
public:
    Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<GameState> _gameState, std::shared_ptr<ResourceManager> _resourceManager);

    void run();

private:
    std::shared_ptr<GameState> gameState;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<InputHandler> inputHandler;
    std::shared_ptr<ResourceManager> resourceManager;
};
