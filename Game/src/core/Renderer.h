#pragma once

#include <memory>

#include "../game/GameState.h"
#include "Settings.h"

class Renderer
{
public:
    Renderer(std::shared_ptr<GameState> _gameState) : gameState(_gameState)
    {
    }

    virtual void init(std::shared_ptr<Settings> settings) = 0;

    virtual void processActionQueue() = 0;

    virtual void render() = 0;

    virtual void shutdown() = 0;

    virtual bool isRunning() = 0;

protected:
    std::shared_ptr<GameState> gameState;
};
