#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<ApplicationDelegate> _delegate) : delegate(_delegate) {}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event) {
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            delegate->placeTarget();
        }
    }

    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.Key == irr::KEY_ESCAPE) {
            delegate->saveLevel();
            delegate->quit();
        }

        if (event.KeyInput.Key == irr::KEY_F2) {
            delegate->saveLevel();
        }
    }

    return false;
}
