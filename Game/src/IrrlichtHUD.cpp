#include "IrrlichtHUD.h"

IrrlichtHUD::IrrlichtHUD(irr::video::IVideoDriver* _driver, irr::gui::IGUIEnvironment* _guienv,
        std::shared_ptr<GameState> _gameState) :
        driver(_driver),
        guienv(_guienv),
        gameState(_gameState),
        bulletTexture(nullptr),
        targetTexture(nullptr),
        statsFont(nullptr)
{
}

void IrrlichtHUD::init()
{
    bulletTexture = driver->getTexture("resources/sprites/bullet.png");
    targetTexture = driver->getTexture("resources/sprites/chicken_head.png");

    statsFont = guienv->getFont("resources/fonts/candyshop_24px.xml");
}

void IrrlichtHUD::render()
{
    renderAmmo();
    renderTargets();
    renderTime();
}

void IrrlichtHUD::renderAmmo()
{
    for (int i = 0; i < gameState->getPlayerState()->getCurrentAmmo(); ++i)
    {
        driver->draw2DImage(
                bulletTexture,
                irr::core::position2d<irr::s32>(30 + (i * (bulletTexture->getOriginalSize().Width + 5)),
                        driver->getScreenSize().Height - bulletTexture->getOriginalSize().Height * 1.25),
                irr::core::rect<irr::s32>(
                        0,
                        0,
                        bulletTexture->getOriginalSize().Width,
                        bulletTexture->getOriginalSize().Height
                ),
                0,
                irr::video::SColor(255, 255, 255, 255),
                true
        );
    }
}

void IrrlichtHUD::renderTargets()
{
    driver->draw2DImage(
            targetTexture,
            irr::core::position2d<irr::s32>(150, 10),
            irr::core::recti(
                    0,
                    0,
                    targetTexture->getOriginalSize().Width,
                    targetTexture->getOriginalSize().Height
            ),
            0,
            irr::video::SColor(255, 255, 255, 255),
            true
    );

    int targetsAlive =
            gameState->getCurrentLevel()->getTargets().size() - gameState->getCurrentScore()->getTargetsEliminated();

    std::wostringstream targetsStr;
    targetsStr << "x" << targetsAlive;

    statsFont->draw(
            targetsStr.str().c_str(),
            irr::core::rect<irr::s32>(
                    150 + targetTexture->getOriginalSize().Width * 0.75,
                    10 + targetTexture->getOriginalSize().Height * 0.75,
                    150 + targetTexture->getOriginalSize().Height * 0.75 + 70,
                    10 + targetTexture->getOriginalSize().Height * 0.75 + 50
            ),
            TARGET_COUNT_COLOR
    );
}

void IrrlichtHUD::renderTime()
{
    unsigned long currentTime = gameState->getCurrentScore()->getCurrentTime();

    unsigned int minutes = currentTime / 100 / 60;
    unsigned int seconds = (currentTime / 100) % 60;

    std::wostringstream timeStr;

    if (minutes < 10)
    {
        timeStr << 0;
    }

    timeStr << minutes;
    timeStr << ":";

    if (seconds < 10)
    {
        timeStr << 0;
    }

    timeStr << seconds;;

    statsFont->draw(
            timeStr.str().c_str(),
            irr::core::rect<irr::s32>(
                    10,
                    10,
                    75,
                    60
            ),
            TARGET_COUNT_COLOR
    );
}
