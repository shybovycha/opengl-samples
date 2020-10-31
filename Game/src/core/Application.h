#pragma once

#include <memory>

#include "../game/GameState.h"
#include "ActionDispatcher.h"
#include "ResourceManager.h"
#include "Renderer.h"

class Application
{
public:
    Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<GameState> _gameState,
            std::shared_ptr<ResourceManager> _resourceManager, std::shared_ptr<ActionDispatcher> _actionDispatcher);

    void run();

private:
    std::shared_ptr<GameState> gameState;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<ActionDispatcher> actionDispatcher;
    std::shared_ptr<ResourceManager> resourceManager;
};
