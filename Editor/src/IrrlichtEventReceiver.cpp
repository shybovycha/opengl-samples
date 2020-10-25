#include "IrrlichtEventReceiver.h"

IrrlichtEventReceiver::IrrlichtEventReceiver(std::shared_ptr<ApplicationDelegate> _delegate) : delegate(_delegate)
{
}

bool IrrlichtEventReceiver::OnEvent(const irr::SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT)
    {
        // CTRL+S saves levels file
        if (event.KeyInput.Key == irr::KEY_KEY_S && event.KeyInput.Control)
        {
            delegate->saveLevels();
        }

        return false;
    }

    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
    {
        if (event.MouseInput.Event == irr::EMIE_MOUSE_MOVED)
        {
            delegate->updateSelectedNodeMovement(event.MouseInput.isLeftPressed());
        }
    }

    if (event.EventType == irr::EET_GUI_EVENT)
    {
        if (event.GUIEvent.EventType == irr::gui::EGET_FILE_SELECTED)
        {
            auto dialog = reinterpret_cast<irr::gui::IGUIFileOpenDialog*>(event.GUIEvent.Caller);
            auto dialogId = static_cast<GUIElementId>(dialog->getID());

            switch (dialogId)
            {
            case GUIElementId::SAVE_LEVELS_DIALOG:
                delegate->saveLevels(dialog->getFileName());
                break;
            case GUIElementId::LOAD_LEVELS_DIALOG:
                delegate->loadLevels(dialog->getFileName());
                break;
            case GUIElementId::LOAD_LEVEL_MESH_DIALOG:
                delegate->addLevel(dialog->getFileName());
                break;
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED)
        {
            auto dialog = reinterpret_cast<irr::gui::IGUIFileOpenDialog*>(event.GUIEvent.Caller);
            auto dialogId = static_cast<GUIElementId>(dialog->getID());

            switch (dialogId)
            {
            case GUIElementId::ABOUT_DIALOG:
                delegate->closeAboutWindow();
                break;

            case GUIElementId::LOAD_LEVELS_DIALOG:
                delegate->closeLoadLevelsDialog();
                break;

            case GUIElementId::SAVE_LEVELS_DIALOG:
                delegate->closeSaveLevelsDialog();
                break;
            case GUIElementId::LOAD_LEVEL_MESH_DIALOG:
                delegate->closeLoadLevelMeshDialog();
                break;
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED)
        {
            auto button = reinterpret_cast<irr::gui::IGUIButton*>(event.GUIEvent.Caller);
            const auto buttonId = static_cast<GUIElementId>(button->getID());

            switch (buttonId)
            {
            case GUIElementId::QUIT:
                delegate->quit();
                break;

            case GUIElementId::SAVE_LEVELS:
                delegate->openSaveLevelsDialog();
                break;

            case GUIElementId::LOAD_LEVELS:
                delegate->openLoadLevelsDialog();
                break;

            case GUIElementId::ABOUT:
                delegate->openAboutWindow();
                break;

            case GUIElementId::ADD_LEVEL:
                delegate->openLoadLevelMeshDialog();
                break;

            case GUIElementId::ADD_TARGET:
                delegate->addTarget();
                break;

            case GUIElementId::ADD_LIGHT:
                delegate->addLight();
                break;

            case GUIElementId::DELETE_SELECTED:
                delegate->deleteSelectedEntity();
                break;
            }

            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_TREEVIEW_NODE_SELECT)
        {
            delegate->gameManagerNodeSelected();

            return false;
        }
    }

    return false;
}
