#include "core/ActionDispatcher.h"
#include "core/Application.h"
#include "renderer/IrrlichtRenderer.h"
#include "core/ModernResourceManager.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

/*
    --------------------------------------------

    This part of code is modern, written with few best practices in mind.
    It also introduces some significant changes to the game architecture (like game state,
    loading levels from files, bringing some sane format for game resources files, etc.).
    This is essentially a new version of the game, taking into consideration all the knowledge
    acquired in last 13 years.
    Feel free to blame the author for beating Web for past decade and the global crisis(-es)
    going on in the world outside if you find this code shit.

    (c) Artem Shubovych, 10 September 2020.

    --------------------------------------------
*/

int main()
{
    std::shared_ptr<ResourceManager> resourceManager = std::make_shared<ModernResourceManager>();
    std::shared_ptr<GameState> gameState = std::make_shared<GameState>();
    std::shared_ptr<ActionDispatcher> actionDispatcher = std::make_shared<ActionDispatcher>(gameState);
    std::shared_ptr<Renderer> renderer = std::make_shared<IrrlichtRenderer>(gameState, actionDispatcher);

    std::shared_ptr<Application> application = std::make_shared<Application>(renderer, gameState, resourceManager,
            actionDispatcher);

    application->run();

    return 0;
}
