#pragma once

#include <memory>
#include <sstream>

#include <irrlicht.h>

#include "../game/GameState.h"

const irr::video::SColor TARGET_COUNT_COLOR(255, 232, 141, 40);

class IrrlichtHUD
{
public:
    IrrlichtHUD(irr::video::IVideoDriver* _driver, irr::gui::IGUIEnvironment* _guienv,
            std::shared_ptr<GameState> _gameState);

    void init();

    void render();

private:
    void renderAmmo();

    void renderTargets();

    void renderTime();

    irr::video::IVideoDriver* driver;
    irr::gui::IGUIEnvironment* guienv;

    irr::video::ITexture* bulletTexture;
    irr::video::ITexture* targetTexture;

    irr::gui::IGUIFont* statsFont;

    std::shared_ptr<GameState> gameState;
};
