#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<ApplicationDelegate> _delegate) : delegate(_delegate) {}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event) {
    //// if (event.EventType == irr::EET_GUI_EVENT && event.EventType == irr::gui::EGET_WINDOW)
    //if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
    //    if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
    //        delegate->placeTarget();
    //    }

    //    return false;
    //}

    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        // CTRL+S saves levels file
        if (event.KeyInput.Key == irr::KEY_KEY_S && event.KeyInput.Control) {
            // TODO: if filename is not set - open dialog and save filename as a member; otherwise - just write the data to the file
            delegate->openSaveLevelsDialog();
        }

        return false;
    }

    if (event.EventType == irr::EET_GUI_EVENT) {
        if (event.GUIEvent.EventType == irr::gui::EGET_FILE_SELECTED) {
            irr::gui::IGUIFileOpenDialog* dialog = (irr::gui::IGUIFileOpenDialog*) event.GUIEvent.Caller;

            if (dialog->getID() == static_cast<irr::s32>(GuiElementID::SAVE_LEVELS_DIALOG)) {
                delegate->saveLevels(dialog->getFileName());
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVELS_DIALOG)) {
                delegate->loadLevels(dialog->getFileName());
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED) {
            irr::gui::IGUIFileOpenDialog* dialog = (irr::gui::IGUIFileOpenDialog*) event.GUIEvent.Caller;

            if (dialog->getID() == static_cast<irr::s32>(GuiElementID::ABOUT_DIALOG)) {
                delegate->closeAboutWindow();
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVELS_DIALOG)) {
                delegate->closeLoadLevelsDialog();
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::SAVE_LEVELS_DIALOG)) {
                delegate->closeSaveLevelsDialog();
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
            if (event.GUIEvent.Caller->getID() == static_cast<irr::s32>(GuiElementID::QUIT)) {
                delegate->quit();
            }

            if (event.GUIEvent.Caller->getID() == static_cast<irr::s32>(GuiElementID::SAVE_LEVELS)) {
                delegate->openSaveLevelsDialog();
            }

            if (event.GUIEvent.Caller->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVELS)) {
                delegate->openLoadLevelsDialog();
            }

            if (event.GUIEvent.Caller->getID() == static_cast<irr::s32>(GuiElementID::ABOUT)) {
                delegate->openAboutWindow();
            }

            if (event.GUIEvent.Caller->getID() == static_cast<irr::s32>(GuiElementID::ADD_LEVEL)) {
                delegate->addLevel();
            }

            if (event.GUIEvent.Caller->getID() == static_cast<irr::s32>(GuiElementID::ADD_TARGET)) {
                delegate->placeTarget();
            }
        }
    }

    return false;
}
