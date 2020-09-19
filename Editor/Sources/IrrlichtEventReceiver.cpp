#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<ApplicationDelegate> _delegate) : delegate(_delegate) {}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event) {
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
            irr::gui::IGUIFileOpenDialog* dialog = reinterpret_cast<irr::gui::IGUIFileOpenDialog*>(event.GUIEvent.Caller);

            if (dialog->getID() == static_cast<irr::s32>(GuiElementID::SAVE_LEVELS_DIALOG)) {
                delegate->saveLevels(dialog->getFileName());
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVELS_DIALOG)) {
                delegate->loadLevels(dialog->getFileName());
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVEL_MESH_DIALOG)) {
                delegate->addLevel(dialog->getFileName());
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED) {
            irr::gui::IGUIFileOpenDialog* dialog = reinterpret_cast<irr::gui::IGUIFileOpenDialog*>(event.GUIEvent.Caller);

            if (dialog->getID() == static_cast<irr::s32>(GuiElementID::ABOUT_DIALOG)) {
                delegate->closeAboutWindow();
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVELS_DIALOG)) {
                delegate->closeLoadLevelsDialog();
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::SAVE_LEVELS_DIALOG)) {
                delegate->closeSaveLevelsDialog();
            }
            else if (dialog->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVEL_MESH_DIALOG)) {
                delegate->closeLoadLevelMeshDialog();
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
            irr::gui::IGUIButton* button = reinterpret_cast<irr::gui::IGUIButton*>(event.GUIEvent.Caller);
            if (button->getID() == static_cast<irr::s32>(GuiElementID::QUIT)) {
                delegate->quit();
            }

            if (button->getID() == static_cast<irr::s32>(GuiElementID::SAVE_LEVELS)) {
                delegate->openSaveLevelsDialog();
            }

            if (button->getID() == static_cast<irr::s32>(GuiElementID::LOAD_LEVELS)) {
                delegate->openLoadLevelsDialog();
            }

            if (button->getID() == static_cast<irr::s32>(GuiElementID::ABOUT)) {
                delegate->openAboutWindow();
            }

            if (button->getID() == static_cast<irr::s32>(GuiElementID::ADD_LEVEL)) {
                delegate->openLoadLevelMeshDialog();
            }

            if (button->getID() == static_cast<irr::s32>(GuiElementID::ADD_TARGET)) {
                delegate->placeTarget();
            }
        }
    }

    return false;
}
