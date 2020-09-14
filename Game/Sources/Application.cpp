#include "Application.h"

Application::Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<GameState> _gameState, std::shared_ptr<ResourceManager> _resourceManager, std::shared_ptr<ActionDispatcher> _actionDispatcher) :
    renderer(std::move(_renderer)),
    gameState(std::move(_gameState)),
    actionDispatcher(std::move(_actionDispatcher)),
    resourceManager(std::move(_resourceManager))
{}

void Application::run() {
    actionDispatcher->levelsLoaded(resourceManager->loadLevels());

    renderer->init(resourceManager->loadSettings());

    while (renderer->isRunning()) {
        renderer->processActionQueue();
        renderer->render();
    }

    renderer->shutdown();
}
